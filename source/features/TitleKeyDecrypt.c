#include "TitleKeyDecrypt.h"
#include "common.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "fs.h"
#include "sdmmc.h"
#include "CTRDecryptor.h"
#include "crypto.h"

#define BUF1 (u8*)0x21000000
#define TITLES (u8*)0x22000000

// From https://github.com/profi200/Project_CTR/blob/master/makerom/pki/prod.h#L19
static const u8 common_keyy[6][16] = {
    {0xD0, 0x7B, 0x33, 0x7F, 0x9C, 0xA4, 0x38, 0x59, 0x32, 0xA2, 0xE2, 0x57, 0x23, 0x23, 0x2E, 0xB9} , // 0 - eShop Titles
    {0x0C, 0x76, 0x72, 0x30, 0xF0, 0x99, 0x8F, 0x1C, 0x46, 0x82, 0x82, 0x02, 0xFA, 0xAC, 0xBE, 0x4C} , // 1 - System Titles
    {0xC4, 0x75, 0xCB, 0x3A, 0xB8, 0xC7, 0x88, 0xBB, 0x57, 0x5E, 0x12, 0xA1, 0x09, 0x07, 0xB8, 0xA4} , // 2
    {0xE4, 0x86, 0xEE, 0xE3, 0xD0, 0xC0, 0x9C, 0x90, 0x2F, 0x66, 0x86, 0xD4, 0xC0, 0x6F, 0x64, 0x9F} , // 3
    {0xED, 0x31, 0xBA, 0x9C, 0x04, 0xB0, 0x67, 0x50, 0x6C, 0x44, 0x97, 0xA3, 0x5B, 0x78, 0x04, 0xFC} , // 4
    {0x5E, 0x66, 0x99, 0x8A, 0xB4, 0xE8, 0x93, 0x16, 0x06, 0x85, 0x0F, 0xD7, 0xA1, 0x6D, 0xD7, 0x55} , // 5
};

int nTicket = 0;
int nKey = 0;
int nullbyte = 0;
	
int isEqual(u8* tid1, u8*tid2){
	for(int i = 0; i < 8; i++){
		if(tid1[i] != tid2[i]) return 0;
	}
	return 1;
}

int isAlreadyDumped(u8* titleid){
	if(nKey == 0) return 0;
	for(int i = 0; i < nKey; i++){
		u8 *stored = TITLES + 8*i;
		if(isEqual(stored, titleid)){
			return 1;
		}
	}
	return 0;
}

unsigned int DecryptTitleKey(unsigned char* titleid, unsigned char* key, unsigned int index)
{
	u8 ctr[16] __attribute__((aligned(32)));
	u8 keyY[16] __attribute__((aligned(32)));
	u8 titleId[8] __attribute__((aligned(32)));
	memcpy(titleId, titleid, 8);
		
	memset(ctr, 0, 16);
	memcpy(ctr, titleId, 8);
	set_ctr(AES_BIG_INPUT|AES_NORMAL_INPUT, ctr);
	memcpy(keyY, (void *) common_keyy[index], 16);
	setup_aeskey(0x3D, AES_BIG_INPUT|AES_NORMAL_INPUT, keyY);
	use_aeskey(0x3D);
	aes_decrypt(key, key, ctr, 1, AES_CBC_DECRYPT_MODE);
	return 0;
}

void DecryptTitleKeys(){
	ConsoleInit();
	ConsoleAddText("Title Key Dumper\n");
	File dump;
	ConsoleAddText("Opening ticket.db..."); ConsoleShow();
	FileOpen(&dump, "decTitleKeys.bin", 1);
	int tick_size = 0xD0000;
	nTicket = 0; nKey = 0;
	
	for(int i = 0; nTicket < 2; i++){
		nand_readsectors(i, 1, BUF1, CTRNAND);
		if(*((char*)BUF1) == 'T' && *((char*)BUF1 + 1) == 'I' && *((char*)BUF1 + 2) == 'C' && *((char*)BUF1 + 3) == 'K'){
			if(nTicket == 0){
				ConsoleAddText("Dumping & Decrypting keys...");
				ConsoleShow();
			}
			nand_readsectors(i, tick_size/0x200, BUF1, CTRNAND);
			nTicket++;
			for(int j = 0; j < tick_size; j++){
				if(!strcmp((char*)BUF1 + j, "Root-CA00000003-XS0000000c")){
					u8 *titleid = BUF1 + j + 0x9C;
					u32 kindex = *(BUF1 + j + 0xB1);
					u8 Key[16]; memcpy(Key, BUF1 + j + 0x7F, 16);
					if( !isAlreadyDumped(titleid)){
						memcpy(TITLES + nKey*8, titleid, 8);
						FileWrite(&dump, &kindex, 4, 0x10 + nKey*0x20);
						FileWrite(&dump, &nullbyte, 4, 0x10 + nKey*0x20 + 4);
						FileWrite(&dump, titleid, 8, 0x10 + nKey*0x20 + 8);
						DecryptTitleKey(titleid, Key, kindex);
						FileWrite(&dump, Key, 16, 0x10 + nKey*0x20 + 16);
						nKey++;
					}
				}
			}
			i += 0x11BE200/0x200;
		}
	}
			
	FileWrite(&dump, &nKey, 4, 0); FileWrite(&dump, &nullbyte, 4, 4); FileWrite(&dump, &nullbyte, 4, 8); FileWrite(&dump, &nullbyte, 4, 12);
	FileClose(&dump);
	ConsoleAddText("\nPress A to exit"); ConsoleShow();
	WaitForButton(BUTTON_A);
}

void DumpTicketDb(){
	ConsoleInit();
	ConsoleAddText("Ticket Dumper\n");
	File dump;
	int tick_size = 0xD0000;			//Standardized a size, after this offset no tickets are found
	nTicket = 0;
	
	ConsoleAddText("Dumping ticket.bin ..."); ConsoleShow();
	FileOpen(&dump, "ticket.bin", 1);
	for(int i = 0; nTicket < 2; i++){
		nand_readsectors(i, 1, BUF1, CTRNAND);
		if(*((char*)BUF1) == 'T' && *((char*)BUF1 + 1) == 'I' && *((char*)BUF1 + 2) == 'C' && *((char*)BUF1 + 3) == 'K'){
			nand_readsectors(i, tick_size/0x200, BUF1, CTRNAND);
			FileWrite(&dump, BUF1, tick_size, nTicket*tick_size);
			nTicket++;		
			i += 0x11BE200/0x200;
		}
	}
	FileClose(&dump); nTicket = 0;
	
	if(checkEmuNAND()){
		ConsoleAddText("Dumping ticket_emu.bin ..."); ConsoleShow();
		FileOpen(&dump, "ticket_emu.bin", 1);
		for(int i = 0; nTicket < 2; i++){
			emunand_readsectors(i, 1, BUF1, CTRNAND);
			if(*((char*)BUF1) == 'T' && *((char*)BUF1 + 1) == 'I' && *((char*)BUF1 + 2) == 'C' && *((char*)BUF1 + 3) == 'K'){
				emunand_readsectors(i, tick_size/0x200, BUF1, CTRNAND);
				FileWrite(&dump, BUF1, tick_size, nTicket*tick_size);
				nTicket++;		
				i += 0x11BE200/0x200;
			}
		}
		FileClose(&dump); nTicket = 0;	
	}
	
	ConsoleAddText("\nPress A to exit"); ConsoleShow();
	WaitForButton(BUTTON_A);

}


