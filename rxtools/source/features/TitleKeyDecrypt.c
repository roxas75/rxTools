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
#include "CTRDecryptor.h"
#include "crypto.h"
#include "stdio.h"

#define BUF1 (uint8_t*)0x21000000
#define TITLES (uint8_t*)0x22000000

#define PROGRESS_WIDTH	16

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

int DecryptTitleKey(uint8_t *titleid, uint8_t *key, uint32_t index) {
	const size_t blockSize = 16;
	static struct {
		uint8_t key[blockSize];
		uint32_t pad;
	} *keyYList = NULL;
	uint8_t ctr[blockSize] __attribute__((aligned(32)));
	uint8_t keyY[blockSize] __attribute__((aligned(32)));
	uint8_t titleId[8] __attribute__((aligned(32)));
	uintptr_t p;

	memcpy(titleId, titleid, 8);
	memset(ctr, 0, blockSize);
	memcpy(ctr, titleId, 8);
	set_ctr(AES_BIG_INPUT | AES_NORMAL_INPUT, ctr);

	if (keyYList == NULL) {
		p = 0x08080000;
		while (((uint8_t *)p)[0] != 0xD0 || ((uint8_t *)p)[1] != 0x7B) {
			p++;
			if (p >= 0x080A0000)
				return 1;
		}

		keyYList = (void *)p;
	}

	memcpy(keyY, keyYList[index].key, sizeof(keyY));
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
	const TCHAR *filename=_T("rxTools/decTitleKeys.bin");
	print(strings[STR_OPENING], "ticket.db");
	FileOpen(&dump, filename, 1);
	uint32_t tick_size = 0xD0000;     //Chunk size
	nKey = 0; int nullbyte = 0;
	if (FileOpen(&tick, _T("1:dbs/ticket.db"), 0)) {
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
	const TCHAR *filename = _T("rxTools/encTitleKeys.bin");
	const TCHAR *filename2 = _T("rxTools/decTitleKeys.bin");
	const TCHAR *filename3 = _T("rxTools/decTitleKeysA.bin");
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

	f_lseek(&dump, 0x10);
	for (i = 0; i < keycount; i ++) {
		rr = f_read(&tick, line, sizeof(line), &br);
		if ((rr != FR_OK)||(br != sizeof(line))) {
			print(L"\n");
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
			print(L"\n");
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
			print(L"\n");
			print(strings[STR_ERROR_WRITING], filename3);
			print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
			ConsoleShow();
			WaitForButton(BUTTON_A);
			return;
		}
		DecryptTitleKey(titleid, key, kindex);
		rr = f_write(&dump, key, sizeof(key), &br);
		if ((rr != FR_OK)||(br != sizeof(key))) {
			print(L"\n");
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
			print(L"\r%s", progressbar);
			ConsoleShow();
		}
	}
	percent++;
	wcsncpy(progress, strings[STR_PROGRESS_OK], wcslen(strings[STR_PROGRESS_OK]));
	progress += wcslen(strings[STR_PROGRESS_OK]);
	print(L"\r%s\n", progressbar);
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

int getTitleKey(uint8_t *TitleKey, uint32_t low, uint32_t high, int drive) {
	File tick;
	uint32_t tid_low = ((low >> 24) & 0xff) | ((low << 8) & 0xff0000) | ((low >> 8) & 0xff00) | ((low << 24) & 0xff000000);
	uint32_t tid_high = ((high >> 24) & 0xff) | ((high << 8) & 0xff0000) | ((high >> 8) & 0xff00) | ((high << 24) & 0xff000000);
	uint32_t tick_size = 0x200;     //Chunk size

	wchar_t path[_MAX_LFN] = {0};
	int r;

	swprintf(path, _MAX_LFN, L"%d:dbs/ticket.db", drive);

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
						r = DecryptTitleKey(titleid, Key, kindex);
						if (!r)
							memcpy(TitleKey, Key, 16);
						FileClose(&tick);
						return r;
					}
				}
			}
		}
		FileClose(&tick);
	}
	return 1;
}

static FRESULT seekRead(FIL *fp, DWORD ofs, void *buff, UINT btr)
{
	FRESULT r;
	UINT br;

	r = f_lseek(fp, ofs);
	if (r != FR_OK)
		return r;

	r = f_read(fp, buff, btr, &br);
	return br < btr ? (r == FR_OK ? EOF : r) : FR_OK;
}

#define CETK_MEMBER_SIZE(member) (sizeof(((TicketHdr *)NULL)->member))
#define CETK_READ_MEMBER(fp, member, buff)	\
	(seekRead((fp), 0x140 + offsetof(TicketHdr, member), buff,	\
		CETK_MEMBER_SIZE(member)))

int getTitleKeyWithCetk(uint8_t dst[16], const TCHAR *path)
{
	uint8_t id[CETK_MEMBER_SIZE(titleId)];
	uint8_t index;
	FRESULT r;
	FIL f;

	r = f_open(&f, path, FA_READ);
	if (r != FR_OK)
		return r;

	r = CETK_READ_MEMBER(&f, titleKey, dst);
	if (r != FR_OK) {
		f_close(&f);
		return r;
	}

	r = CETK_READ_MEMBER(&f, titleId, id);
	if (r != FR_OK) {
		f_close(&f);
		return r;
	}

	r = CETK_READ_MEMBER(&f, keyIndex, &index);
	if (r != FR_OK) {
		f_close(&f);
		return r;
	}

	f_close(&f);
	return DecryptTitleKey(id, dst, index);
}
