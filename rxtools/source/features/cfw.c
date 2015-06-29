#include "cfw.h"
#include "common.h"
#include "hid.h"
#include "filepack.h"
#include "console.h"
#include "aes.h"
#include "sdmmc.h"
#include "fs.h"
#include "ncch.h"
#include "CTRDecryptor.h"
#include "TitleKeyDecrypt.h"

#define FIRM_ADDR 0x24000000
#define ARMBXR4	0x47204C00	

char tmp[256];
unsigned int emuNandMounted = 0;
void (*_softreset)() = 0x080F0000;

void firmlaunch(u8* firm){
	memcpy(FIRM_ADDR, firm, 0x200000); 	//Fixed size, no FIRM right now is that big
	memcpy(0x080F0000, GetFilePack("reboot.bin"), 0x8000);
	_softreset();
}

void applyPatch(unsigned char* file, unsigned char* patch){
	unsigned char* start = patch;
	unsigned int ndiff = *((unsigned int*)patch); patch += 4;
	for(int i = 0; i < ndiff; i++){
		unsigned int off = *((unsigned int*)patch); patch += 4;
		unsigned int len = *((unsigned int*)patch); patch += 4;
		if(1){
			for(int j = 0; j < len; j++){
				*(file + j + off) = *patch++;
			}
		}else{
			patch += len;
		}
		while(((unsigned int)patch % 4)!= 0) patch++;
	}
}

u8* decryptFirmTitle(u8* title, unsigned int size, unsigned int tid){
	u8 key[0x10] = {0};
	u8 iv[0x10] = {0};
	GetTitleKey(&key[0], 0x00040138, tid);
	aes_context aes_ctxt;
	aes_setkey_dec(&aes_ctxt, &key[0], 0x80);
	aes_crypt_cbc(&aes_ctxt, AES_DECRYPT, size, iv, title, title);
	ctr_ncchheader NCCH; u8 CTR[16]; PartitionInfo INFO;
	NCCH = *((ctr_ncchheader*)title); ncch_get_counter(NCCH, CTR, 2);
	INFO.ctr = CTR; INFO.buffer = title + getle32(NCCH.exefsoffset)*0x200; INFO.keyY = NCCH.signature; INFO.size = size; INFO.keyslot = 0x2C;
	DecryptPartition(&INFO);
	u8* firm = (u8*)(INFO.buffer + 0x200);
	return firm;
}

void setFirmMode(int mode){ //0 : SysNand, 1 : EmuNand
	if(!checkEmuNAND()) mode = 0;	//forcing to SysNand if there is no EmuNand
	File firm;
	u32 mmc_original[] = { 0x000D0004, 0x001E0017 };
	u32 nat_emuwrite[] = { ARMBXR4, 0x0801A4C0 };
	u32 nat_emuread[] = { ARMBXR4, 0x0801A5B0 };
	if(FileOpen(&firm, "rxtools/data/0004013800000002.bin", 0)){
		FileWrite(&firm, mode ? &nat_emuwrite : &mmc_original, 8, 0xCCF2C);
		FileWrite(&firm, mode ? &nat_emuread : &mmc_original, 8, 0xCCF6C);
		FileClose(&firm);
	}
}

int rxMode(int mode){	//0 : SysNand, 1 : EmuNand
	setFirmMode(mode);
	File myFile;
	u8* native_firm = (u8*)0x21000000;
	if(FileOpen(&myFile, "rxtools/data/0004013800000002.bin", 0)){
		FileRead(&myFile, native_firm, 0xF0000, 0);
		FileClose(&myFile);
		firmlaunch(native_firm);
	}
	return -1;
}

void rxModeSys(){
    ConsoleInit();
    ConsoleSetTitle("rxMode - Booting in SysNand");
    print("Loading...\n"); ConsoleShow();
	rxMode(0);
	print("Cannot boot in rxMode.\n\nPress A to exit\n"); ConsoleShow();
	WaitForButton(BUTTON_A);
}

void rxModeEmu(){
	if(!checkEmuNAND()) rxModeSys();
	else{
		ConsoleInit();
		ConsoleSetTitle("rxMode - Booting in EmuNand");
		print("Loading...\n"); ConsoleShow();
		rxMode(1);
		print("Cannot boot in rxMode.\n\nPress A to exit\n"); ConsoleShow();
		WaitForButton(BUTTON_A);
	}
}

//Just patches signatures check, loads in sysnand
void DevMode(){
    u8* firm = 0x24000000;
    nand_readsectors(0, 0xF0000/0x200, firm, FIRM0);
    if(strncmp((char*)firm, "FIRM", 4))
    nand_readsectors(0, 0xF0000/0x200, firm, FIRM1);

    unsigned char sign1[] = {0xC1, 0x17, 0x49, 0x1C, 0x31, 0xD0, 0x68, 0x46, 0x01, 0x78, 0x40, 0x1C, 0x00, 0x29, 0x10, 0xD1};
    unsigned char sign2[] = {0xC0, 0x1C, 0x76, 0xE7, 0x20, 0x00, 0x74, 0xE7, 0x22, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F};
    unsigned char patch1[] = { 0x00, 0x20, 0x4E, 0xB0, 0x70, 0xBD };
    unsigned char patch2[] = { 0x00, 0x20};

    for(int i = 0; i < 0xF0000; i++){
        if(!memcmp(firm + i, sign1, 16)){
            memcpy(firm + i, patch1, 6);
        }
        if(!memcmp(firm + i, sign2, 16)){
            memcpy(firm + i, patch2, 2);
        }
    }
    memcpy(0x080F0000, GetFilePack("reboot.bin"), 0x8000);
	_softreset();
}
