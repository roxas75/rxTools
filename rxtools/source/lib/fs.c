#include "common.h"
#include "fs.h"
#include "fatfs/ff.h"
#include "CTRDecryptor.h"
#include "crypto.h"
#include "sdmmc.h"

////////////////////////////////////////////////////////////////Basic FileSystem Operations
static FATFS fs[3];

bool InitFS() {
	FSNandInitCrypto();
	int res = 1;
	if (f_mount(&fs[0], "0:", 0) != FR_OK) { res = 0; }
	if (f_mount(&fs[1], "1:", 0) != FR_OK) { res = 0; }
	if (f_mount(&fs[2], "2:", 0) != FR_OK) { res = 0; }
	return res;
}

void DeinitFS() {
	f_mount(NULL, "0:", 0);
	f_mount(NULL, "1:", 0);
	f_mount(NULL, "2:", 0);
}

bool FileOpen(File *Handle, const char *path, bool truncate) {
	unsigned flags = FA_READ | FA_WRITE;
	flags |= truncate ? FA_CREATE_ALWAYS : FA_OPEN_EXISTING; //: FA_OPEN_ALWAYS;
	bool ret = (f_open(Handle, path, flags) == FR_OK);
	if (f_tell(Handle)) { f_lseek(Handle, 0); } //Only seek to head if not
	f_sync(Handle);
	return ret;
}

size_t FileRead(File *Handle, void *buf, size_t size, size_t foffset) {
	UINT bytes_read = 0;
	if (f_tell(Handle) != foffset) { f_lseek(Handle, foffset); } //Avoid crazy lseeks
	f_read(Handle, buf, size, &bytes_read);
	return bytes_read;
}

size_t FileWrite(File *Handle, void *buf, size_t size, size_t foffset) {
	UINT bytes_written = 0;
	if (f_tell(Handle) != foffset) { f_lseek(Handle, foffset); } //Avoid crazy lseeks
	f_write(Handle, buf, size, &bytes_written);
	f_sync(Handle);
	return bytes_written;
}

size_t FileGetSize(File *Handle) {
	return f_size(Handle);
}

void FileClose(File *Handle) {
	f_close(Handle);
}
////////////////////////////////////////////////////////////////Advanced FileSystem Operations
/** Copy Source File (source) to Target (target).
  * @param  target Target file, will be created if not exists.
  * @param  source Source file, must exists.
  * @retval Compounded value of (STEP<<8)|FR_xx, so it contains real reasons.
  * @note   directly FATFS calls. FATFS return value only ranges from 0 to 19.
  */
u32 FSFileCopy(char *target, char *source) {
	FIL src, dst;
	u32 step = 0; //Tells you where it failed
	FRESULT retfatfs = 0; //FATFS return value
	u32 blockSize = 0x4000;
	u8 *buffer = (u8 *)0x26000200; //Temp buffer for transfer.
	UINT byteI = 0, byteO = 0; //Bytes that read or written
	retfatfs = f_open(&src, source, FA_OPEN_EXISTING);
	if (retfatfs != FR_OK) {step = 1; goto closeExit;}
	retfatfs = f_open(&dst, target, FA_CREATE_ALWAYS);
	if (retfatfs != FR_OK) {step = 2; goto closeExit;}
	u32 totalSize = src.fsize;
	//If source file has no contents, nothing to be copied.
	if (!totalSize) { goto closeExit; }
	while (totalSize) {
		if (totalSize < blockSize) { blockSize = totalSize; }
		//FR_OK, FR_DISK_ERR, FR_INT_ERR, FR_INVALID_OBJECT, FR_TIMEOUT
		retfatfs = f_read(&src, buffer, blockSize, &byteI);
		if (retfatfs != FR_OK) {step = 3; goto closeExit;}
		//Unexpected
		if (byteI != blockSize) {step = 4; goto closeExit;}
		//FR_OK, FR_DISK_ERR, FR_INT_ERR, FR_INVALID_OBJECT, FR_TIMEOUT
		retfatfs = f_write(&dst, buffer, blockSize, &byteO);
		if (retfatfs != FR_OK) {step = 5; goto closeExit;}
		//Unexpected
		if (byteO != blockSize) {step = 6; goto closeExit;}
		totalSize -= blockSize;
	}
	step = 0; //Success return value.
closeExit:
	f_close(&src);
	f_close(&dst);
	return (step << 8) | retfatfs;
}

int FileCopy(char *dest, char *source) {
	File out;
	File in;
	if (!FileOpen(&in, source, 0)) { return -1; }
	if (!FileOpen(&out, dest, 1)) { return -1; }
	unsigned int size = FileGetSize(&in);
	if (size == 0) {
		FileClose(&in);
		FileClose(&out);
		return -1;
	}
	int pos = 0, res = 1;
	unsigned int i, chunk_size = 0x4000;
	u8 *buf = (u8 *)0x26000200;
	for (i = 0; i < size; i += chunk_size) {
		if (chunk_size > (size - i)) { chunk_size = (size - i); }
		int rb = FileRead(&in, buf, chunk_size, i);
		if (rb != chunk_size) {
			/* error or eof */
			res = -1;
			break;
		}
		int wb = FileWrite(&out, buf, chunk_size, i);
		if (wb != chunk_size) {
			/* error or disk full */
			res = -2;
			break;
		}
		pos += wb;
	}
	FileClose(&in);
	FileClose(&out);
	return res;
}

////////////////////////////////////////////////////////////////NAND Operations
u8 NANDCTR[16];
int sysversion = 0;

int GetSystemVersion() {
	return sysversion;
}
/**CTR offsets with versions*/
const u8 *fsVersionCTRs[] = {
	(u8 *)0x080D7CAC, //4.x
	(u8 *)0x080D858C, //5.x
	(u8 *)0x080D748C, //6.x
	(u8 *)0x080D740C, //7.x
	(u8 *)0x080D74CC, //8.x
	(u8 *)0x080D794C //9.x
};
const u32 fsVersionCTRsLength = sizeof(fsVersionCTRs) / sizeof(u8 *);
/**Copy NAND Cypto to our region.*/
u8 *FSNandFindCTR(void) {
	for (u32 i = 0; i < fsVersionCTRsLength; i++)
		if (*(u32 *)fsVersionCTRs[i] == 0x5C980) {
			return (u8 *)(fsVersionCTRs[i] + 0x30);
		}
	// If value not in previous list start memory scanning (test range)
	for (u8 *c = (u8 *)0x080D8FFF; c > (u8 *)0x08000000; c--)
		if (*(u32 *)c == 0x5C980 && *(u32 *)(c + 1) == 0x800005C9) {
			return c + 0x30;
		}
	return NULL;
}
/**Copy NAND Cypto to our region.*/
void FSNandInitCrypto(void) {
	u8 *ctrStart = FSNandFindCTR();
	u8 *ctrStore = NANDCTR;
	u8 i = 16; //CTR length
	if (!ctrStart) { return; } //Avoid copying from NULL
	ctrStart = ctrStart + 15;
	//The CTR is stored backwards in memory.
	while (i --) { *(ctrStore++) = *(ctrStart--); }
}


int checkEmuNAND() {
	u8 *check = (u8 *)0x26000000;
	sdmmc_sdcard_readsectors(0x3AF00000 / 0x200, 1, check);
	if (*((char *)check + 0x100) == 'N' && *((char *)check + 0x101) == 'C' && *((char *)check + 0x102) == 'S' && *((char *)check + 0x103) == 'D') {
		return 0x3AF00000;
	} else {
		sdmmc_sdcard_readsectors(0x3BA00000 / 0x200, 1, check);
		if (*((char *)check + 0x100) == 'N' && *((char *)check + 0x101) == 'C' && *((char *)check + 0x102) == 'S' && *((char *)check + 0x103) == 'D') {
			return 0x3BA00000;
		} else {
			return 0;
		}
	}
}

void GetNANDCTR(u8 *ctr) {
	for (int i = 0; i < 16; i++) { *(ctr + i) = NANDCTR[i]; }
}

void nand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition) {
	PartitionInfo info;
	u8 myCtr[16];
	for (int i = 0; i < 16; i++) { myCtr[i] = NANDCTR[i]; }
	info.ctr = myCtr; info.buffer = out; info.size = numsectors * 0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition / 16);
	switch (partition) {
		case TWLN	  : info.keyslot = 0x3; break;
		case TWLP	  : info.keyslot = 0x3; break;
		case AGB_SAVE : info.keyslot = 0x7; break;
		case FIRM0    : info.keyslot = 0x6; break;
		case FIRM1    : info.keyslot = 0x6; break;
		case CTRNAND  : info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no * 0x20);
	sdmmc_nand_readsectors(sector_no + partition / 0x200, numsectors, out);
	DecryptPartition(&info);
}

void nand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition) {
	PartitionInfo info;
	u8 myCtr[16];
	for (int i = 0; i < 16; i++) { myCtr[i] = NANDCTR[i]; }
	info.ctr = myCtr; info.buffer = out; info.size = numsectors * 0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition / 16);
	switch (partition) {
		case TWLN	  : info.keyslot = 0x3; break;
		case TWLP	  : info.keyslot = 0x3; break;
		case AGB_SAVE : info.keyslot = 0x7; break;
		case FIRM0    : info.keyslot = 0x6; break;
		case FIRM1    : info.keyslot = 0x6; break;
		case CTRNAND  : info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no * 0x20);
	DecryptPartition(&info);
	sdmmc_nand_writesectors(sector_no + partition / 0x200, numsectors, out);	//Stubbed, i don't wanna risk
}

void emunand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition) {
	PartitionInfo info;
	u8 myCtr[16];
	for (int i = 0; i < 16; i++) { myCtr[i] = NANDCTR[i]; }
	info.ctr = myCtr; info.buffer = out; info.size = numsectors * 0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition / 16);
	switch (partition) {
		case TWLN	  : info.keyslot = 0x3; break;
		case TWLP	  : info.keyslot = 0x3; break;
		case AGB_SAVE : info.keyslot = 0x7; break;
		case FIRM0    : info.keyslot = 0x6; break;
		case FIRM1    : info.keyslot = 0x6; break;
		case CTRNAND  : info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no * 0x20);
	sdmmc_sdcard_readsectors(sector_no + partition / 0x200, numsectors, out);
	DecryptPartition(&info);
}

void emunand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition) {
	PartitionInfo info;
	u8 myCtr[16];
	for (int i = 0; i < 16; i++) { myCtr[i] = NANDCTR[i]; }
	info.ctr = myCtr; info.buffer = out; info.size = numsectors * 0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition / 16);
	switch (partition) {
		case TWLN	  : info.keyslot = 0x3; break;
		case TWLP	  : info.keyslot = 0x3; break;
		case AGB_SAVE : info.keyslot = 0x7; break;
		case FIRM0    : info.keyslot = 0x6; break;
		case FIRM1    : info.keyslot = 0x6; break;
		case CTRNAND  : info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no * 0x20);
	DecryptPartition(&info);
	sdmmc_sdcard_writesectors(sector_no + partition / 0x200, numsectors, out);	//Stubbed, i don't wanna risk
}
