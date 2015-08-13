/*
 * Copyright (C) 2015 The PASTA Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CTRDecryptor.h"
#include "screenshot.h"
#include "fs.h"
#include "fatfs/ff.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "ncch.h"
#include "crypto.h"
#include "stdio.h"
#include "lang.h"
#include "menu.h"

#define BUFFER_ADDR ((uint8_t*)0x21000000)
#define BLOCK_SIZE  (8*1024*1024)

char str[100];

uint32_t DecryptPartition(PartitionInfo* info){
	if(info->keyY != NULL)
		setup_aeskey(info->keyslot, AES_BIG_INPUT|AES_NORMAL_INPUT, info->keyY);
	use_aeskey(info->keyslot);

	uint8_t ctr[16] __attribute__((aligned(32)));
	memcpy(ctr, info->ctr, 16);

	uint32_t size_bytes = info->size;
	for (uint32_t i = 0; i < size_bytes; i += BLOCK_SIZE) {
		uint32_t j;
		for (j = 0; (j < BLOCK_SIZE) && (i+j < size_bytes); j+= 16) {
			set_ctr(AES_BIG_INPUT|AES_NORMAL_INPUT, ctr);
			aes_decrypt((void*)info->buffer+j, (void*)info->buffer+j, ctr, 1, AES_CTR_MODE);
			add_ctr(ctr, 1);
			TryScreenShot(); //Putting it here allows us to take screenshots at any decryption point, since everyting loops in this
		}
	}
	return 0;
}

void ProcessExeFS(PartitionInfo* info){ //We expect Exefs to take just a block. Why? No exefs right now reached 8MB.
	if(info->keyslot == 0x2C){
		DecryptPartition(info);
	}else if(info->keyslot == 0x25){  //The new keyX is a bit tricky, 'couse only .code is encrypted with it
		PartitionInfo myInfo;
		memcpy((void*)&myInfo, (void*)info, sizeof(PartitionInfo));
		uint8_t OriginalCTR[16]; memcpy(OriginalCTR, info->ctr, 16);
		myInfo.keyslot = 0x2C; myInfo.size = 0x200;
		DecryptPartition(&myInfo); add_ctr(myInfo.ctr, 0x200 / 16);
		if(myInfo.buffer[0] == '.' && myInfo.buffer[1] == 'c' && myInfo.buffer[2] == 'o' && myInfo.buffer[3] == 'd' && myInfo.buffer[4] == 'e'){
			//The 7.xKey encrypted .code partition
			uint32_t codeSize = *((unsigned int*)(myInfo.buffer + 0x0C));
			uint32_t nextSection = *((unsigned int*)(myInfo.buffer + 0x18)) + 0x200;
			myInfo.buffer += 0x200; myInfo.size = codeSize; myInfo.keyslot = 0x25;
			DecryptPartition(&myInfo);
			//The rest is normally encrypted
			memcpy((void*)&myInfo, (void*)info, sizeof(PartitionInfo));
			myInfo.buffer += nextSection; myInfo.size -= nextSection; myInfo.keyslot = 0x2C;
			myInfo.ctr = OriginalCTR;
			add_ctr(myInfo.ctr, nextSection/16);
			DecryptPartition(&myInfo);
		}else{
			myInfo.size = info->size-0x200;
			myInfo.buffer += 0x200;
			DecryptPartition(&myInfo);
		}
	}
}

int ProcessCTR(char* path){
	PartitionInfo myInfo;
	File myFile;
	if(FileOpen(&myFile, path, 0)){
		ConsoleInit();
		ConsoleSetTitle(strings[STR_DECRYPT], strings[STR_CTR]);
		unsigned int ncch_base = 0x100;
		unsigned char magic[] = { 0, 0, 0, 0, 0};
		FileRead(&myFile, magic, 4, ncch_base);
		if(magic[0] == 'N' && magic[1] == 'C' && magic[2] == 'S' && magic[3] == 'D'){
			ncch_base = 0x4000;
			FileRead(&myFile, magic, 4, ncch_base+0x100);
			if(!(magic[0] == 'N' && magic[1] == 'C' && magic[2] == 'C' && magic[3] == 'H')){
				FileClose(&myFile);
				return 2;
			}
		}else if(magic[0] == 'N' && magic[1] == 'C' && magic[2] == 'C' && magic[3] == 'H'){
			ncch_base = 0x0;
		}else{
			FileClose(&myFile);
			return 2;
		}
		ctr_ncchheader NCCH; unsigned int mediaunitsize = 0x200;
		FileRead(&myFile, &NCCH, 0x200, ncch_base);

		print(L"%s\n", (char*)NCCH.productcode);
		unsigned int NEWCRYPTO = 0, CRYPTO = 1;
		if(NCCH.flags[3] != 0) NEWCRYPTO = 1;
		if(NCCH.flags[7] & 4) CRYPTO = 0;
		if(NEWCRYPTO){
			print(strings[STR_CRYPTO_TYPE], strings[STR_KEY7]);
		}else if(CRYPTO){
			print(strings[STR_CRYPTO_TYPE], strings[STR_SECURE]);
		}else{
			print(strings[STR_CRYPTO_TYPE], strings[STR_NONE]);
			print(strings[STR_COMPLETED]);
			FileClose(&myFile);
			ConsoleShow();
			return 3;
		}

		uint8_t CTR[16];
		if(getle32(NCCH.extendedheadersize) > 0){
			print(strings[STR_DECRYPTING], strings[STR_EXHEADER]);
			ConsoleShow();
			ncch_get_counter(NCCH, CTR, 1);
			FileRead(&myFile, BUFFER_ADDR, 0x800, ncch_base + 0x200);
			myInfo.buffer = BUFFER_ADDR;
			myInfo.size = 0x800;
			myInfo.keyslot = 0x2C;
			myInfo.ctr = CTR;
			myInfo.keyY = NCCH.signature;
			DecryptPartition(&myInfo);
			FileWrite(&myFile, BUFFER_ADDR, 0x800, ncch_base + 0x200);
		}
		if(getle32(NCCH.exefssize) > 0){
			print(strings[STR_DECRYPTING], strings[STR_EXEFS]);
			ConsoleShow();
			ncch_get_counter(NCCH, CTR, 2);
			myInfo.buffer = BUFFER_ADDR;
			myInfo.keyslot = NEWCRYPTO ? 0x25 : 0x2C;
			myInfo.ctr = CTR;
			myInfo.keyY = NCCH.signature;

			size_t bytesRead = FileRead(&myFile, BUFFER_ADDR, getle32(NCCH.exefssize) * mediaunitsize, ncch_base + getle32(NCCH.exefsoffset) * mediaunitsize);
			myInfo.size = bytesRead;
			ProcessExeFS(&myInfo); //Explanation at function definition
			FileWrite(&myFile, BUFFER_ADDR, getle32(NCCH.exefssize) * mediaunitsize, ncch_base + getle32(NCCH.exefsoffset) * mediaunitsize);
		}
		if(getle32(NCCH.romfssize) > 0){
			print(strings[STR_DECRYPTING], strings[STR_ROMFS]);
			ConsoleShow();
			ncch_get_counter(NCCH, CTR, 3);
			myInfo.buffer = BUFFER_ADDR;
			myInfo.keyslot = NEWCRYPTO ? 0x25 : 0x2C;
			myInfo.ctr = CTR;
			myInfo.keyY = NCCH.signature;
			for(int i = 0; i < (getle32(NCCH.romfssize) * mediaunitsize + BLOCK_SIZE - 1) / BLOCK_SIZE; i++){
				print(L"%3d%%", (int)((i*BLOCK_SIZE)/(getle32(NCCH.romfssize) * mediaunitsize/ 100)));
				for(int j = 0; j < 4; j++) ConsolePrev();
				ConsoleShow();
				size_t bytesRead = FileRead(&myFile, BUFFER_ADDR, i*BLOCK_SIZE <= (getle32(NCCH.romfssize) * mediaunitsize) ? BLOCK_SIZE : (getle32(NCCH.romfssize) * mediaunitsize) % BLOCK_SIZE, ncch_base + getle32(NCCH.romfsoffset) * mediaunitsize + i*BLOCK_SIZE);
				myInfo.size = bytesRead;
				DecryptPartition(&myInfo);
				add_ctr(myInfo.ctr, bytesRead/16);
				FileWrite(&myFile, BUFFER_ADDR, bytesRead, ncch_base + getle32(NCCH.romfsoffset) * mediaunitsize + i*BLOCK_SIZE);
			}
			print(L"\n");
		}
		NCCH.flags[7] |= 4; //Disable encryption
		NCCH.flags[3] = 0;  //Disable 7.XKey usage
		FileWrite(&myFile, &NCCH, 0x200, ncch_base);
		if(ncch_base == 0x4000) FileWrite(&myFile, ((uint8_t*)&NCCH) + 0x100, 0x100, 0x1100);   //Only for NCSD
		FileClose(&myFile);
		print(strings[STR_COMPLETED]);
		ConsoleShow();
		return 0;
	}else return 1;
}

int ExploreFolders(char* folder){
	int nfiles = 0;
	DIR myDir;
	FILINFO curInfo;
	memset(&myDir, 0, sizeof(DIR));
	memset(&curInfo, 0, sizeof(FILINFO));
	FILINFO *myInfo = &curInfo;

	myInfo->fname[0] = 'A';
	while(f_opendir(&myDir, folder) != FR_OK);
	for(int i = 0; myInfo->fname[0] != 0; i++){
		if( f_readdir(&myDir, myInfo)) break;
		if(myInfo->fname[0] == '.' || !strcmp(myInfo->fname, "NINTEN~1")) continue;

		char path[1024];
		sprintf(path, "%s/%s", folder, myInfo->fname);
		if(path[strlen(path) - 1] == '/') break;

		if(myInfo->fattrib & AM_DIR){
			nfiles += ExploreFolders(path);
		}else if(true){
			if(ProcessCTR(path) == 0){
				nfiles++;
			}
		}

	}
	f_closedir(&myDir);
	return nfiles;
}

void CTRDecryptor(){
	ConsoleInit();
	ConsoleSetTitle(strings[STR_DECRYPT], strings[STR_CTR]);
	ConsoleShow();

	int nfiles = ExploreFolders("");

	ConsoleInit();
	print(strings[STR_DECRYPTED], nfiles, strings[STR_FILES]);
	print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);

	ConsoleShow();
	WaitForButton(BUTTON_A);
}
