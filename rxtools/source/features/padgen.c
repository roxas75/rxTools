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

#include <stdlib.h>
#include <string.h>

#include "fs.h"
#include "draw.h"
#include "lang.h"
#include "padgen.h"
#include "crypto.h"
#include "console.h"
#include "hid.h"

void PadGen(){
	ConsoleInit();
	ConsoleSetTitle(strings[STR_GENERATE], strings[STR_XORPAD]);
	NcchPadgen();
	ConsoleShow();
	SdPadgen();
	ConsoleShow();

	print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

uint32_t NcchPadgen()
{
	uint32_t result;
	File pf;
	NcchInfo *info = (NcchInfo*)0x20316000;

	const char *filename = "/ncchinfo.bin";
	wchar_t wfilename[14];
	mbstowcs(wfilename, filename, 14);
	if (!FileOpen(&pf, filename, 0)) {
		print(strings[STR_ERROR_OPENING], filename+1);
		return 1;
	}
	FileRead(&pf, info, 16, 0);

	if (info->ncch_info_version != 0xF0000003) {
        	print(strings[STR_WRONG], filename+1, strings[STR_VERSION]);
		return 0;
	}
	if (!info->n_entries || info->n_entries > MAXENTRIES) {
        	print(strings[STR_WRONG], filename+1, strings[STR_ENTRIES_COUNT]);
		return 0;
	}
	FileRead(&pf, info->entries, info->n_entries * sizeof(NcchInfoEntry), 16);
	FileClose(&pf);

	print(strings[STR_PROCESSING], wfilename+1);
	ConsoleShow();
	for(uint32_t i = 0; i < info->n_entries; i++) {
		PadInfo padInfo = {.setKeyY = 1, .size_mb = info->entries[i].size_mb};
		memcpy(padInfo.CTR, info->entries[i].CTR, 16);
		memcpy(padInfo.keyY, info->entries[i].keyY, 16);
		memcpy(padInfo.filename, info->entries[i].filename, 112);

		if(info->entries[i].uses7xCrypto)
			padInfo.keyslot = 0x25;
		else
			padInfo.keyslot = 0x2C;

		result = CreatePad(&padInfo, i);
		if (result) return 1;
	}

	return 0;
}

uint32_t SdPadgen()
{
	size_t bytesRead;
	uint32_t result;
	File fp;
	SdInfo *info = (SdInfo*)0x20316000;

	uint8_t movable_seed[0x120] = {0};
	const char *filename = "/movable.sed";
	wchar_t wfilename[13];
	mbstowcs(wfilename, filename, 13);
	// Load console 0x34 keyY from movable.sed if present on SD card
	if (FileOpen(&fp, filename, 0)) {
		bytesRead = FileRead(&fp, &movable_seed, 0x120, 0);
		FileClose(&fp);
		if (bytesRead != 0x120) {
			print(strings[STR_WRONG], filename+1, strings[STR_SIZE]);
			return 1;
		}
		if (memcmp(movable_seed, "SEED", 4) != 0) {
			print(strings[STR_WRONG], filename+1, strings[STR_CONTENT]);
			return 1;
		}
		setup_aeskey(0x34, AES_BIG_INPUT|AES_NORMAL_INPUT, &movable_seed[0x110]);
		use_aeskey(0x34);
	}

	filename = "/SDinfo.bin";
	if (!FileOpen(&fp, filename, 0)) {
		print(strings[STR_ERROR_OPENING], filename+1);
		return 1;
	}
	bytesRead = FileRead(&fp, info, 4, 0);

	if (!info->n_entries || info->n_entries > MAXENTRIES) {
        	print(strings[STR_WRONG], filename+1, strings[STR_ENTRIES_COUNT]);
		return 1;
	}

       	print(strings[STR_PROCESSING], wfilename+1);
	ConsoleShow();

	bytesRead = FileRead(&fp, info->entries, info->n_entries * sizeof(SdInfoEntry), 4);
	FileClose(&fp);

	for(uint32_t i = 0; i < info->n_entries; i++) {
		PadInfo padInfo = {.keyslot = 0x34, .setKeyY = 0, .size_mb = info->entries[i].size_mb};
		memcpy(padInfo.CTR, info->entries[i].CTR, 16);
		memcpy(padInfo.filename, info->entries[i].filename, 180);

		result = CreatePad(&padInfo, i);
		if (result)
			return 1;
	}

	return 0;
}

static const uint8_t zero_buf[16] __attribute__((aligned(16))) = {0};

uint32_t CreatePad(PadInfo *info, int index)
{
	File pf;
	#define BUFFER_ADDR ((volatile uint8_t*)0x21000000)
	#define BLOCK_SIZE  (4*1024*1024)

	if (!FileOpen(&pf, info->filename, 1))
		return 1;

	if(info->setKeyY != 0)
		setup_aeskey(info->keyslot, AES_BIG_INPUT|AES_NORMAL_INPUT, info->keyY);
	use_aeskey(info->keyslot);

	uint8_t ctr[16] __attribute__((aligned(32)));
	memcpy(ctr, info->CTR, 16);

	uint32_t size_bytes = info->size_mb*1024*1024;
	uint32_t size_100 = size_bytes/100;
	uint32_t seekpos = 0;
	for (uint32_t i = 0; i < size_bytes; i += BLOCK_SIZE) {
		uint32_t j;
		for (j = 0; (j < BLOCK_SIZE) && (i+j < size_bytes); j+= 16) {
			set_ctr(AES_BIG_INPUT|AES_NORMAL_INPUT, ctr);
			aes_decrypt((void*)zero_buf, (void*)BUFFER_ADDR+j, ctr, 1, AES_CTR_MODE);
			add_ctr(ctr, 1);
		}

		print(strings[STR_GENERATING], strings[STR_PAD]);
		print(L"%i : %i%%", index, (i+j)/size_100);
		ConsolePrevLine();
		ConsolePrevLine();
		ConsoleShow();
		FileWrite(&pf, (void*)BUFFER_ADDR, j, seekpos);
		seekpos += j;
	}

	FileClose(&pf);
	return 0;
}
