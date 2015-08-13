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

#include <stdint.h>
#include <string.h>
#include "TitleKeyDecrypt.h"
#include "console.h"
#include "draw.h"
#include "lang.h"
#include "hid.h"
#include "fs.h"
#include "fatfs/sdmmc.h"
#include "CTRDecryptor.h"
#include "crypto.h"
#include "stdio.h"

#define BUF1 (uint8_t*)0x21000000
#define TITLES (uint8_t*)0x22000000

#define PROGRESS_WIDTH	16

// From https://github.com/profi200/Project_CTR/blob/master/makerom/pki/prod.h#L19
static const uint8_t common_keyy[6][16] = {
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

int isEqual(uint8_t *tid1, uint8_t *tid2) {
	for (int i = 0; i < 8; i++) {
		if (tid1[i] != tid2[i]) { return 0; }
	}
	return 1;
}

int isAlreadyDumped(uint8_t *titleid) {
	if (nKey == 0) { return 0; }
	for (int i = 0; i < nKey; i++) {
		uint8_t *stored = TITLES + 8 * i;
		if (isEqual(stored, titleid)) {
			return 1;
		}
	}
	return 0;
}

uint32_t DecryptTitleKey(uint8_t *titleid, uint8_t *key, uint32_t index) {
	uint8_t ctr[16] __attribute__((aligned(32)));
	uint8_t keyY[16] __attribute__((aligned(32)));
	uint8_t titleId[8] __attribute__((aligned(32)));
	memcpy(titleId, titleid, 8);
	memset(ctr, 0, 16);
	memcpy(ctr, titleId, 8);
	set_ctr(AES_BIG_INPUT | AES_NORMAL_INPUT, ctr);
	memcpy(keyY, (void *) common_keyy[index], 16);
	setup_aeskey(0x3D, AES_BIG_INPUT | AES_NORMAL_INPUT, keyY);
	use_aeskey(0x3D);
	aes_decrypt(key, key, ctr, 1, AES_CBC_DECRYPT_MODE);
	return 0;
}

void DecryptTitleKeys() {
	ConsoleInit();
	ConsoleSetTitle(strings[STR_DECRYPT], strings[STR_TITLE_KEYS]);
	File tick;
	File dump;
	const char *filename="rxTools/decTitleKeys.bin";
	print(strings[STR_OPENING], "ticket.db");
	FileOpen(&dump, filename, 1);
	uint32_t tick_size = 0xD0000;     //Chunk size
	nKey = 0; int nullbyte = 0;
	if (FileOpen(&tick, "1:dbs/ticket.db", 0)) {
		print(strings[STR_DECRYPTING], strings[STR_TITLE_KEYS], filename);
		ConsoleShow();
		uint8_t *buf = BUF1;
		int pos = 0;
		for (;;) {
			int rb = FileRead(&tick, buf, tick_size, pos);
			if (rb == 0) { break; } /* error or eof */
			pos += rb;
			for (int j = 0; j < tick_size; j++) {
				if (!strcmp((char *)buf + j, "Root-CA00000003-XS0000000c")) {
					uint8_t *titleid = buf + j + 0x9C;
					uint32_t kindex = *(buf + j + 0xB1);
					uint8_t Key[16]; memcpy(Key, BUF1 + j + 0x7F, 16);
					if (!isAlreadyDumped(titleid)) {
						memcpy(TITLES + nKey * 8, titleid, 8);
						FileWrite(&dump, &kindex, 4, 0x10 + nKey * 0x20);
						FileWrite(&dump, &nullbyte, 4, 0x10 + nKey * 0x20 + 4);
						FileWrite(&dump, titleid, 8, 0x10 + nKey * 0x20 + 8);
						DecryptTitleKey(titleid, Key, kindex);
						FileWrite(&dump, Key, 16, 0x10 + nKey * 0x20 + 16);
						nKey++;
					}
				}
			}
		}
		FileClose(&dump);
		FileClose(&tick);
	} else {
		print(strings[STR_FAILED]);
	}
	FileWrite(&dump, &nKey, 4, 0);
	FileWrite(&dump, &nullbyte, 4, 4);
	FileWrite(&dump, &nullbyte, 4, 8);
	FileWrite(&dump, &nullbyte, 4, 12);
	FileClose(&dump);
	print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

/** This decrypts the encTitleKeys.bin inside RxTools directory on SD. */
void DecryptTitleKeyFile(void) {
	ConsoleInit();
	ConsoleSetTitle(strings[STR_DECRYPT], strings[STR_TITLE_KEYS_FILE]);
	FIL tick, dump;
	FRESULT rr = 0;
	UINT br = 0;
	const char *filename = "rxTools/encTitleKeys.bin";
	const char *filename2 = "rxTools/decTitleKeys.bin";
	const char *filename3 = "rxTools/decTitleKeysA.bin";
	print(strings[STR_OPENING], filename);
	ConsoleShow();
	//decTitleKeys.bin that generated from other stuff can be handled streamly.
	FileOpen(&dump, filename2, 1);
	rr = f_open(&dump, filename3, FA_WRITE | FA_CREATE_ALWAYS);
	if (rr != FR_OK) {
		f_close(&dump);
		print(strings[STR_ERROR_OPENING], filename3);
		print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
		ConsoleShow();
		WaitForButton(BUTTON_A);
		return;
	}
	rr = f_open(&tick, filename, FA_READ | FA_OPEN_EXISTING);
	if (rr != FR_OK) {
		f_close(&tick); f_close(&dump);
		print(strings[STR_ERROR_OPENING], filename);
		print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
		ConsoleShow();
		WaitForButton(BUTTON_A);
		return;
	}

	uint32_t line[4] = {0,};
	uint32_t keycount = 0, i = 0;
	uint32_t kindex = 0, nkeys = 0;
	uint8_t titleid[8] = {0,};
	uint8_t key[16] = {0,};
	wchar_t progressbar[41] = {0,};
	wchar_t* progress = progressbar;
	for(i=0; i<PROGRESS_WIDTH; i++)
		wcscat(progressbar, strings[STR_PROGRESS]);
	uint8_t percent = 0;

	rr = f_read(&tick, line, sizeof(line), &br);
	if ((rr != FR_OK)||(br != sizeof(line)))
	{
		print(strings[STR_ERROR_READING], filename);
		print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
		ConsoleShow();
		WaitForButton(BUTTON_A);
		return;
	}
	keycount = line[0];
	if (f_size(&tick) != 0x10 + 0x20*keycount) {
		print(strings[STR_KEYS_MISMATCH], keycount, 0x10 + 0x20*keycount, f_size(&tick));
		print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
		ConsoleShow();
		WaitForButton(BUTTON_A);
		return;
	}

	print(progressbar);
	ConsoleShow();
	ConsolePrevLine();

	f_lseek(&dump, 0x10);
	for (i = 0; i < keycount; i ++) {
		rr = f_read(&tick, line, sizeof(line), &br);
		if ((rr != FR_OK)||(br != sizeof(line))) {
			print(strings[STR_ERROR_READING], filename);
			print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
			ConsoleShow();
			WaitForButton(BUTTON_A);
			return;
		}
		kindex = line[0]; //Title Type which decides which common KeyY.
		memcpy(titleid, line + 2, 8);
		rr = f_read(&tick, line, sizeof(line), &br);
		if ((rr != FR_OK)||(br != sizeof(line))) {
			print(strings[STR_ERROR_READING], filename);
			print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
			ConsoleShow();
			WaitForButton(BUTTON_A);
			return;
		}
		memcpy(key, line, 16);

		memcpy(TITLES + nkeys * 8, titleid, 8);
		//Craft one line per time.
		line[0] = kindex;
		line[1] = 0;
		memcpy(line + 2, titleid, 8);
		rr = f_write(&dump, line, sizeof(line), &br);
		if ((rr != FR_OK)||(br != sizeof(line))) {
			print(strings[STR_ERROR_WRITING], filename3);
			print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
			ConsoleShow();
			WaitForButton(BUTTON_A);
			return;
		}
		DecryptTitleKey(titleid, key, kindex);
		rr = f_write(&dump, key, sizeof(key), &br);
		if ((rr != FR_OK)||(br != sizeof(key))) {
			print(strings[STR_ERROR_WRITING], filename3);
			print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
			ConsoleShow();
			WaitForButton(BUTTON_A);
			return;
		}
		nkeys ++;

		if (percent < i*PROGRESS_WIDTH/keycount) {
			percent++;
			wcsncpy(progress, strings[STR_PROGRESS_OK], wcslen(strings[STR_PROGRESS_OK]));
			progress += wcslen(strings[STR_PROGRESS_OK]);
			print(progressbar);
			ConsoleShow();
			ConsolePrevLine();
		}
	}
	percent++;
	wcsncpy(progress, strings[STR_PROGRESS_OK], wcslen(strings[STR_PROGRESS_OK]));
	progress += wcslen(strings[STR_PROGRESS_OK]);
	print(progressbar);
	ConsoleShow();
	rr = f_write(&dump, line, sizeof(line), &br);
	if ((rr != FR_OK)||(br != sizeof(line)))
	{
		print(strings[STR_ERROR_WRITING], filename3);
		print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
		ConsoleShow();
		WaitForButton(BUTTON_A);
		return;
	}
	f_close(&tick); f_close(&dump);
	print(strings[STR_DECRYPTED], nkeys, strings[STR_TITLE_KEYS]);
	print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
	ConsoleShow();
	WaitForButton(BUTTON_A);
	return;
}

int GetTitleKey(uint8_t *TitleKey, uint32_t low, uint32_t high, int drive) {
	File tick;
	uint32_t tid_low = ((low >> 24) & 0xff) | ((low << 8) & 0xff0000) | ((low >> 8) & 0xff00) | ((low << 24) & 0xff000000);
	uint32_t tid_high = ((high >> 24) & 0xff) | ((high << 8) & 0xff0000) | ((high >> 8) & 0xff00) | ((high << 24) & 0xff000000);
	uint32_t tick_size = 0x200;     //Chunk size

	char path[64] = {0};
	sprintf(path, "%d:dbs/ticket.db", drive);

	if (FileOpen(&tick, path, 0)) {
		uint8_t *buf = TITLES;
		int pos = 0;
		for (;;) {
			int rb = FileRead(&tick, buf, tick_size, pos);
			if (rb == 0) { break; } /* error or eof */
			pos += rb;
			if (buf[0] == 'T' && buf[1] == 'I' && buf[2] == 'C' && buf[3] == 'K') {
				tick_size = 0xD0000;
				continue;
			}
			for (int j = 0; j < tick_size; j++) {
				if (!strcmp((char *)buf + j, "Root-CA00000003-XS0000000c")) {
					uint8_t *titleid = buf + j + 0x9C;
					uint32_t kindex = *(buf + j + 0xB1);
					uint8_t Key[16]; memcpy(Key, buf + j + 0x7F, 16);
					if (*((uint32_t *)titleid) == tid_low && *((uint32_t *)(titleid + 4)) == tid_high) {
						DecryptTitleKey(titleid, Key, kindex);
						memcpy(TitleKey, Key, 16);
						FileClose(&tick);
						return 1;
					}
				}
			}
		}
		FileClose(&tick);
	}
	return 0;
}
