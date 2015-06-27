#include "common.h"
#include "fs.h"
#include "fatfs/ff.h"
#include "CTRDecryptor.h"
#include "crypto.h"
#include "sdmmc.h"

static FATFS fs[3];
static FIL file;

u8 NANDCTR[16];
int sysversion = 0;

int GetSystemVersion(){
	return sysversion;
}

static u8* FindNandCtr()
{
    static const u8* version_ctrs[] = {
        (u8*)0x080D7CAC,//4.x
        (u8*)0x080D858C,//5.x
        (u8*)0x080D748C,//6.x
        (u8*)0x080D740C,//7.x
        (u8*)0x080D74CC,//8.x
        (u8*)0x080D794C //9.x
    };
    static const u32 version_ctrs_len = sizeof(version_ctrs) / sizeof(u32);

    for (u32 i = 0; i < version_ctrs_len; i++)
        if (*(u32*)version_ctrs[i] == 0x5C980)
            return (u8*)(version_ctrs[i] + 0x30);

    // If value not in previous list start memory scanning (test range)
    for (u8* c = (u8*)0x080D8FFF; c > (u8*)0x08000000; c--)
        if (*(u32*)c == 0x5C980 && *(u32*)(c + 1) == 0x800005C9)
            return c + 0x30;

    return NULL;
}

void InitializeNandCrypto(){
	u8* ctrStart = FindNandCtr();
    for(u32 i = 0; i < 16; i++)
        NANDCTR[i] = *(ctrStart + (15 - i)); //The CTR is stored backwards in memory.
}

bool InitFS()
{
	InitializeNandCrypto();
	int res = 1;
	if(f_mount(&fs[0], "0:", 0) != FR_OK) res = 0;
	if(f_mount(&fs[1], "1:", 0) != FR_OK) res = 0;
	if(f_mount(&fs[2], "2:", 0) != FR_OK);
	return res;
}

void DeinitFS()
{
    f_mount(NULL, "0:", 0);
	f_mount(NULL, "1:", 0);
	f_mount(NULL, "2:", 0);
}

bool FileOpen(File *Handle, const char* path, bool truncate)
{
    unsigned flags = FA_READ | FA_WRITE;
    flags |= truncate ? FA_CREATE_ALWAYS : FA_OPEN_EXISTING; //: FA_OPEN_ALWAYS;
    bool ret = (f_open(Handle, path, flags) == FR_OK);
    f_lseek(Handle, 0);
	f_sync(&file);
    return ret;
}

size_t FileRead(File *Handle, void* buf, size_t size, size_t foffset)
{
    UINT bytes_read = 0;
    f_lseek(Handle, foffset);
    f_read(Handle, buf, size, &bytes_read);
    return bytes_read;
}

size_t FileWrite(File *Handle, void* buf, size_t size, size_t foffset)
{
    UINT bytes_written = 0;
    f_lseek(Handle, foffset);
    f_write(Handle, buf, size, &bytes_written);
	f_sync(&file);
    return bytes_written;
}

size_t FileGetSize(File *Handle)
{
    return f_size(Handle);
}

void FileClose(File *Handle)
{
    f_close(Handle);
}

int FileCopy(char* dest, char* source)
{
	File out;
	File in;
	if (!FileOpen(&in, source, 0)) return -1;
	if (!FileOpen(&out, dest, 1)) return -1;
	
	unsigned int size = FileGetSize(&in);
	if (size == 0)
	{
		FileClose(&in);
		FileClose(&out);
		return -1;
	}
	
	int pos = 0, res = 1;
	unsigned int i, chunk_size = 0x4000;
	unsigned char* buf = 0x26000200;
	
	for (i = 0; i < size; i += chunk_size)
	{
		if (chunk_size > (size - i)) chunk_size = (size - i);
		
		int rb = FileRead(&in, buf, chunk_size, i);
		if (rb != chunk_size)
		{
			/* error or eof */
			res = 0;
			break;
		}
		
		int wb = FileWrite(&out, buf, chunk_size, i);
		if (wb != chunk_size)
		{
			/* error or disk full */
			res = 0;
			break;
		}
		
		pos += wb;
	}
	
	FileClose(&in);
	FileClose(&out);
	return res;
}

int checkEmuNAND(){
	unsigned char* check = 0x26000000;
	sdmmc_sdcard_readsectors(0x3AF00000/0x200, 1, check);
	if(*((char*)check + 0x100) == 'N' && *((char*)check + 0x101) == 'C' && *((char*)check + 0x102) == 'S' && *((char*)check + 0x103) == 'D'){
		return 0x3AF00000;
	}else{
		sdmmc_sdcard_readsectors(0x3BA00000/0x200, 1, check);
		if(*((char*)check + 0x100) == 'N' && *((char*)check + 0x101) == 'C' && *((char*)check + 0x102) == 'S' && *((char*)check + 0x103) == 'D'){
			return 0x3BA00000;
		}else{
			return 0;
		}
	}
}

void GetNANDCTR(u8* ctr){
	for(int i = 0; i < 16; i++) *(ctr + i) = NANDCTR[i];
}

int nand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition){
	PartitionInfo info;
	u8 myCtr[16];
	for(int i = 0; i < 16; i++) myCtr[i] = NANDCTR[i];
	info.ctr = &myCtr; info.buffer = out; info.size = numsectors*0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition/16);
	switch(partition){
		case TWLN	  : info.keyslot = 0x3; break;
		case TWLP	  : info.keyslot = 0x3; break;
		case AGB_SAVE : info.keyslot = 0x7; break;
		case FIRM0    : info.keyslot = 0x6; break;
		case FIRM1    : info.keyslot = 0x6; break;
		case CTRNAND  : info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no*0x20);

	sdmmc_nand_readsectors(sector_no + partition/0x200, numsectors, out);
	DecryptPartition(&info);
}

int nand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition){
	PartitionInfo info;
	u8 myCtr[16];
	for(int i = 0; i < 16; i++) myCtr[i] = NANDCTR[i];
	info.ctr = &myCtr; info.buffer = out; info.size = numsectors*0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition/16);
	switch(partition){
		case TWLN	  : info.keyslot = 0x3; break;
		case TWLP	  : info.keyslot = 0x3; break;
		case AGB_SAVE : info.keyslot = 0x7; break;
		case FIRM0    : info.keyslot = 0x6; break;
		case FIRM1    : info.keyslot = 0x6; break;
		case CTRNAND  : info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no*0x20);

	DecryptPartition(&info);
	sdmmc_nand_writesectors(sector_no + partition/0x200, numsectors, out);	//Stubbed, i don't wanna risk
}

int emunand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition){
	PartitionInfo info;
	u8 myCtr[16];
	for(int i = 0; i < 16; i++) myCtr[i] = NANDCTR[i];
	info.ctr = &myCtr; info.buffer = out; info.size = numsectors*0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition/16);
	switch(partition){
		case TWLN	  : info.keyslot = 0x3; break;
		case TWLP	  : info.keyslot = 0x3; break;
		case AGB_SAVE : info.keyslot = 0x7; break;
		case FIRM0    : info.keyslot = 0x6; break;
		case FIRM1    : info.keyslot = 0x6; break;
		case CTRNAND  : info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no*0x20);

	sdmmc_sdcard_readsectors(sector_no + partition/0x200, numsectors, out);
	DecryptPartition(&info);
}

int emunand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition){
	PartitionInfo info;
	u8 myCtr[16];
	for(int i = 0; i < 16; i++) myCtr[i] = NANDCTR[i];
	info.ctr = &myCtr; info.buffer = out; info.size = numsectors*0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition/16);
	switch(partition){
		case TWLN	  : info.keyslot = 0x3; break;
		case TWLP	  : info.keyslot = 0x3; break;
		case AGB_SAVE : info.keyslot = 0x7; break;
		case FIRM0    : info.keyslot = 0x6; break;
		case FIRM1    : info.keyslot = 0x6; break;
		case CTRNAND  : info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no*0x20);

	DecryptPartition(&info);
	sdmmc_sdcard_writesectors(sector_no + partition/0x200, numsectors, out);	//Stubbed, i don't wanna risk
}
