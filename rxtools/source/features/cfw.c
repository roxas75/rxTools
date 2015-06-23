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

#define FIRMWARE_HASH 0x8116b730 			//Encrypted 9.6 NATIVE_FIRM title
#define FIRM_ADDR 0x24000000

char tmp[256];
unsigned int emuNandMounted = 0;
void (*_softreset)() = 0x080F0000;

void softreset(){
    u8* code = GetFilePack(REBOOT);
	memcpy(0x080F0000, code, 0x8000);
    _softreset();
}

#define emunand_write 0xCCF2C
#define emunand_read  0xCCF6C
void applyPatch(unsigned char* file, unsigned char* patch){
	unsigned char* start = patch;
	unsigned int ndiff = *((unsigned int*)patch); patch += 4;
	for(int i = 0; i < ndiff; i++){
		unsigned int off = *((unsigned int*)patch); patch += 4;
		unsigned int len = *((unsigned int*)patch); patch += 4;
		if(!(!emuNandMounted && (off == emunand_write || off == emunand_read))){
			for(int j = 0; j < len; j++){
				*(file + j + off) = *patch++;
			}
		}else{
			patch += len;
		}
		while(((unsigned int)patch % 4)!= 0) patch++;
	}
}

u8* decryptFirmTitle(u8* title, unsigned int size){
	u8 key[0x10] = {0};
	u8 iv[0x10] = {0};
	GetTitleKey(&key[0], 0x00040138, 0x00000002);
	aes_context aes_ctxt;
	aes_setkey_dec(&aes_ctxt, &key[0], 0x80);
	aes_crypt_cbc(&aes_ctxt, AES_DECRYPT, size, iv, title, title);
	ctr_ncchheader NCCH; u8 CTR[16]; PartitionInfo INFO;
	NCCH = *((ctr_ncchheader*)title); ncch_get_counter(NCCH, CTR, 2);
	INFO.ctr = CTR; INFO.buffer = title + getle32(NCCH.exefsoffset)*0x200; INFO.keyY = NCCH.signature; INFO.size = 0xF0000; INFO.keyslot = 0x2C;
	DecryptPartition(&INFO);
	u8* firm = (u8*)(INFO.buffer + 0x200);
	return firm;
}

int generateCfw(){
	emuNandMounted = checkEmuNAND();
	for(int i = 0; i < 0x100000; i++){
		u32 pad = GetInput();
		if(pad & BUTTON_X) emuNandMounted = 0;
	}//PRESS X FOR SYSNAND DURING BOOT
	
	File title; u8* ptitle = 0x21000000;
	if(!FileOpen(&title, "firmware.bin", 0)){
		print("Cannot find 'firmware.bin'!\n"); ConsoleShow();
		return 0;
	}
	unsigned int title_size = FileGetSize(&title);
	FileRead(&title, ptitle, title_size, 0);
	FileClose(&title);
	if(!CheckHash(ptitle, title_size, FIRMWARE_HASH)){
		print("Corrupted firmware file!\n"); ConsoleShow();
		return 0;
	}
	u8* firmware = decryptFirmTitle(ptitle, title_size);
	u8* Patch = GetFilePack(PATCH);
	applyPatch(firmware, Patch);
	if(!firmware || strncmp((char*)firmware, "FIRM", 4)){
		print("Wrong firmware file!\n"); ConsoleShow();
		return 0;
	}

	memcpy(FIRM_ADDR, firmware, 0xF0000);
	File KeyFile;
	int newKey = 0;
	if(FileOpen(&KeyFile, "/slot0x25KeyX.bin", 0) && GetSystemVersion() < 3){
		newKey = 1;
	}

	//Finalizing some patches
	u8* cfwPatch = FIRM_ADDR;
	for(int i = 0; i < 0xF0000; i+=0x4){
		if(!strcmp((char*)cfwPatch + i, "Shit")){
			if(emuNandMounted){
				memcpy((char*)cfwPatch + i, "RX-E", 4);
			}else{
				memcpy((char*)cfwPatch + i, "RX-S", 4);
			}
		}
		if(!strcmp((char*)cfwPatch + i, "InsertKeyXHere!") && newKey){
			FileRead(&KeyFile, cfwPatch + i, 16, 0);
			FileClose(&KeyFile);
		}
		if(*((unsigned int*)(cfwPatch + i)) == 0xAAAABBBB){
			*((unsigned int*)(cfwPatch + i)) = (checkEmuNAND() / 0x200) - 1;
		}
	}
	//FileOpen(&KeyFile, "cfw_gen.bin", 1); FileWrite(&KeyFile, 0x24000000, 0xF0000, 0); FileClose(&KeyFile);
	//Hardcode the cfw for reboothax
	memcpy(firmware, cfwPatch, 0xF0000);
	//Cool and unusual crypto, huh?
	unsigned int* xorfirm = (unsigned int*)firmware;
	int startXor = 0xDEADBEEF;
	for(int i = 0; i < 0xF0000/4; i++){
		xorfirm[i] ^= startXor;
		startXor += 0x12345678;
	}
	if(FileOpen(&title, "rxTools.dat", 0)){
	    FileWrite(&title, firmware, 0xF0000, 0x200000 );
	    FileClose(&title);
    }
	//Let's go
	return 1;
}

void rxMode_boot(){
	if(generateCfw()){
        softreset();
	}
}

void LaunchCfw(){
    ConsoleInit();
    ConsoleSetTitle("rxMode - Custom Firmware Boot");
    print("Loading...\n"); ConsoleShow();
	
	if(generateCfw()){
        softreset();
	}
	print("\nPress A to exit\n"); ConsoleShow();
	WaitForButton(BUTTON_A);
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
    softreset();
}
