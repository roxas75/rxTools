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
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <elf.h>
#include <reboot.h>
#include "firm.h"
#include "mpcore.h"
#include "hid.h"
#include "lang.h"
#include "console.h"
#include "polarssl/aes.h"
#include "fatfs/ff.h"
#include "fatfs/sdmmc.h"
#include "fs.h"
#include "ncch.h"
#include "draw.h"
#include "menu.h"
#include "fileexplorer.h"
#include "CTRDecryptor.h"
#include "TitleKeyDecrypt.h"
#include "configuration.h"
#include "lang.h"

const char firmPathFmt[] = FIRM_PATH_FMT;
const char firmPatchPathFmt[] = FIRM_PATCH_PATH_FMT;

unsigned int emuNandMounted = 0;
_Noreturn void (* const _softreset)() = (void *)0x080F0000;

_Noreturn void execReboot(uint32_t, void *, uintptr_t, const Elf32_Shdr *);

static FRESULT loadExecReboot()
{
	FIL fd;
	FRESULT r;
	UINT br;

	r = f_open(&fd, SYS_PATH "/reboot.bin", FA_READ);
	if (r != FR_OK)
		return r;

	r = f_read(&fd, (void*)0x080F0000, 0x8000, &br);
	if (r != FR_OK)
		return r;

	f_close(&fd);
	_softreset();
}

static int loadFirm(char *path, UINT *fsz)
{
	FIL fd;
	FRESULT r;

	r = f_open(&fd, path, FA_READ);
	if (r != FR_OK)
		return r;

	r = f_read(&fd, (void *)FIRM_ADDR, f_size(&fd), fsz);
	if (r != FR_OK)
		return r;

	f_close(&fd);

	return ((FirmHdr *)FIRM_ADDR)->magic == 0x4D524946 ? 0 : -1;
}

uint8_t* decryptFirmTitleNcch(uint8_t* title, unsigned int size){
	const size_t sector = 512;
	ctr_ncchheader NCCH;
	uint8_t CTR[16];
	PartitionInfo INFO;
	NCCH = *((ctr_ncchheader*)title);
	if(memcmp(NCCH.magic, "NCCH", 4) != 0) return NULL;
	ncch_get_counter(NCCH, CTR, 2);
	INFO.ctr = CTR; INFO.buffer = title + getle32(NCCH.exefsoffset)*sector; INFO.keyY = NCCH.signature; INFO.size = getle32(NCCH.exefssize)*sector; INFO.keyslot = 0x2C;
	DecryptPartition(&INFO);
	uint8_t* firm = (uint8_t*)(INFO.buffer + 0x200);
	return firm;
}

uint8_t *decryptFirmTitle(uint8_t *title, unsigned int size, uint8_t key[16])
{
	PartitionInfo info;
	Arm9Hdr *hdr;
	uint8_t *firm;
	aes_context aes_ctxt;

	uint8_t iv[16] = { 0 };
	aes_setkey_dec(&aes_ctxt, &key[0], 0x80);
	aes_crypt_cbc(&aes_ctxt, AES_DECRYPT, size, iv, title, title);
	firm = decryptFirmTitleNcch(title, size);

	if (getMpInfo() == MPINFO_KTR) {
		hdr = (void *)(firm + ((FirmHdr *)firm)->segs[2].offset);

		use_aeskey(0x11);
		aes_decrypt(hdr->keyX_0x16, hdr->keyX_0x16, NULL,
			1, AES_ECB_DECRYPT_MODE);

		setup_aeskeyX(0x16, hdr->keyX_0x16);

		info.ctr = hdr->ctr;
		info.buffer = (uint8_t *)hdr + 0x800;
		info.keyY = hdr->keyY;
		info.size = atoi(hdr->size);
		info.keyslot = 0x16;
		DecryptPartition(&info);

	}

	return firm;
}

static void setAgbBios()
{
	File agb_firm;
	char path[64];
	unsigned char svc = (cfgs[CFG_AGB].val.i ? 0x26 : 0x01);

	getFirmPath(path, TID_CTR_AGB_FIRM);
	if (FileOpen(&agb_firm, path, 0))
	{
		FileWrite(&agb_firm, &svc, 1, 0xD7A12);
		FileClose(&agb_firm);
	}
}

int rxMode(int emu)
{
	char path[64];
	const char *shstrtab;
	const wchar_t *msg;
	uint8_t keyx[16];
	uint32_t tid;
	int r, sector;
	Elf32_Ehdr *ehdr;
	Elf32_Shdr *shdr, *btm;
	void *keyxArg;
	FIL fd;
	UINT br, fsz;

	if (emu) {
		sector = checkEmuNAND();
		if (sector == 0) {
			ConsoleInit();
			ConsoleSetTitle(L"EMUNAND NOT FOUND!");
			print(L"The emunand was not found on\n");
			print(L"your SDCard. \n");
			print(L"\n");
			print(L"Press A to boot SYSNAND\n");
			ConsoleShow();

			WaitForButton(BUTTON_A);

			sprintf(path, "/rxTools/Theme/%u/boot.bin", cfgs[CFG_THEME].val.i);
			DrawBottomSplash(path);
		}
	} else
		sector = 0;

	r = getMpInfo();
	switch (r) {
		case MPINFO_KTR:
			tid = TID_KTR_NATIVE_FIRM;
			break;

		case MPINFO_CTR:
			tid = TID_CTR_NATIVE_FIRM;
			break;

		default:
			msg = L"Unknown Platform: %d";
			goto fail;
	}

	setAgbBios();

	if (sysver < 7 && f_open(&fd, "slot0x25KeyX.bin", FA_READ) == FR_OK) {
		f_read(&fd, keyx, sizeof(keyx), &br);
		f_close(&fd);
		keyxArg = keyx;
	} else
		keyxArg = NULL;

	getFirmPath(path, tid);
	r = loadFirm(path, &fsz);
	if (r) {
		msg = L"Failed to load NATIVE_FIRM: %d\n"
			L"Reboot rxTools and try again.\n";
		goto fail;
	}

	((FirmHdr *)FIRM_ADDR)->arm9Entry = 0x0801B01C;

	getFirmPatchPath(path, tid);
	r = f_open(&fd, path, FA_READ);
	if (r != FR_OK)
		goto patchFail;

	r = f_read(&fd, (void *)PATCH_ADDR, PATCH_SIZE, &br);
	if (r != FR_OK)
		goto patchFail;

	f_close(&fd);

	ehdr = (void *)PATCH_ADDR;
	shdr = (void *)(PATCH_ADDR + ehdr->e_shoff);
	shstrtab = (char *)PATCH_ADDR + shdr[ehdr->e_shstrndx].sh_offset;
	for (btm = shdr + ehdr->e_shnum; shdr != btm; shdr++) {
		if (!strcmp(shstrtab + shdr->sh_name, ".patch.p9.reboot.body")) {
			execReboot(sector, keyxArg, ehdr->e_entry, shdr);
			__builtin_unreachable();
		}
	}

	msg = L".patch.p9.reboot.body not found\n"
		L"Please check your installation.\n";
fail:
	ConsoleInit();
	ConsoleSetTitle(L"rxMode");
	print(msg, r);
	print(L"\n");
	print(strings[STR_PRESS_BUTTON_ACTION],
		strings[STR_BUTTON_A], strings[STR_CONTINUE]);
	ConsoleShow();
	WaitForButton(BUTTON_A);

	return r;

patchFail:
	msg = L"Failed to load the patch: %d\n"
		L"Check your installation.\n";
	goto fail;
}

void rxModeWithSplash(int emu)
{
	char s[32];

	sprintf(s, "/rxTools/Theme/%u/boot.bin", cfgs[CFG_THEME].val.i);
	DrawBottomSplash(s);
	rxMode(emu);
}

//Just patches signatures check, loads in sysnand
int PastaMode(){
	/*PastaMode is ready for n3ds BUT there's an unresolved bug which affects nand reading functions, like nand_readsectors(0, 0xF0000 / 0x200, firm, FIRM0);*/

	uint8_t* firm = (void*)FIRM_ADDR;
	nand_readsectors(0, 0xF0000 / 0x200, firm, FIRM0);
	if (strncmp((char*)firm, "FIRM", 4))
		nand_readsectors(0, 0xF0000 / 0x200, firm, FIRM1);

	if(getMpInfo() == MPINFO_CTR)
	{
		//o3ds patches
		unsigned char sign1[] = { 0xC1, 0x17, 0x49, 0x1C, 0x31, 0xD0, 0x68, 0x46, 0x01, 0x78, 0x40, 0x1C, 0x00, 0x29, 0x10, 0xD1 };
		unsigned char sign2[] = { 0xC0, 0x1C, 0x76, 0xE7, 0x20, 0x00, 0x74, 0xE7, 0x22, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F };
		unsigned char patch1[] = { 0x00, 0x20, 0x4E, 0xB0, 0x70, 0xBD };
		unsigned char patch2[] = { 0x00, 0x20 };

		for (int i = 0; i < 0xF0000; i++){
			if (!memcmp(firm + i, sign1, 16)){
				memcpy(firm + i, patch1, 6);
			}
			if (!memcmp(firm + i, sign2, 16)){
				memcpy(firm + i, patch2, 2);
			}
		}
	}
	else
	{
		//new 3ds patches
		uint8_t patch1[] = { 0x6D, 0x20, 0xCE, 0x77 };
		uint8_t patch2[] = { 0x5A, 0xC5, 0x73, 0xC1 };
		memcpy((uint32_t*)0x08052FD8, patch1, 4);
		memcpy((uint32_t*)0x08058804, patch2, 4);
	}

	return loadExecReboot();
}

void FirmLoader(){
	char firm_path[256];

	if (!FileExplorerMain(firm_path, sizeof(firm_path)))
	{
		UINT fsz;
		if (loadFirm(firm_path, &fsz))
		{
			ConsoleInit();
			ConsoleSetTitle(strings[STR_LOAD], strings[STR_FIRMWARE_FILE]);
			print(strings[STR_WRONG], "", strings[STR_FIRMWARE_FILE]);
			print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
			ConsoleShow();
			WaitForButton(BUTTON_A);
			return;
		}

		if (loadExecReboot())
		{
			ConsoleInit();
			ConsoleSetTitle(strings[STR_LOAD], strings[STR_FIRMWARE_FILE]);
			print(strings[STR_ERROR_LAUNCHING], strings[STR_FIRMWARE_FILE]);
			print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
			ConsoleShow();
			WaitForButton(BUTTON_A);
			return;
		}
	}
}
