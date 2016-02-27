/*
 * Copyright (C) 2015-2016 The PASTA Team
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
#include <features/firm.h>
#include <lib/cfg.h>
#include <lib/mpcore.h>
#include <lib/hid.h>
#include <lib/lang.h>
#include <lib/ui/console.h>
#include <lib/polarssl/aes.h>
#include <lib/fatfs/ff.h>
#include <lib/fs.h>
#include <lib/ncch.h>
#include <lib/ui/draw.h>
#include <features/decryption.h>
#include <lib/lang.h>

const wchar_t firmPathFmt[] = _T("") FIRM_PATH_FMT;
const wchar_t firmPatchPathFmt[] = _T("") FIRM_PATCH_PATH_FMT;

_Noreturn void (* const _softreset)() = (void *)0x080F0000;

static FRESULT loadExecReboot()
{
	FIL fd;
	FRESULT r;
	UINT br;

	r = f_open(&fd, _T("") SYS_PATH "/reboot.bin", FA_READ);
	if (r != FR_OK)
		return r;

	r = f_read(&fd, (void*)0x080F0000, 0x8000, &br);
	if (r != FR_OK)
		return r;

	f_close(&fd);
	_softreset();
}

static int loadFirm(TCHAR *path, UINT *fsz)
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

static void setAgbBios()
{
	File agb_firm;
	TCHAR path[64];
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
	const Elf32_Addr line = 32;
	wchar_t path[64];
	const char *shstrtab;
	const wchar_t *msg;
	uint8_t keyx[16];
	uint32_t tid;
	int r, sector;
	Elf32_Ehdr *ehdr;
	Elf32_Shdr *shdr, *btmShdr;
	Elf32_Addr cur, btm;
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

			swprintf(path, _MAX_LFN, L"/rxTools/Theme/%u/boot.bin",
				cfgs[CFG_THEME].val.i);
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

	if (sysver < 7 && f_open(&fd, _T("slot0x25KeyX.bin"), FA_READ) == FR_OK) {
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
	for (btmShdr = shdr + ehdr->e_shnum; shdr != btmShdr; shdr++) {
		if (!strcmp(shstrtab + shdr->sh_name, ".patch.p9.reboot.body")) {
			/* Set MPU area 2.
			   address: 0x0800000, size: 2^0x28 = 2M, enable */
			__asm__ volatile ("mcr p15, 0, %0, c6, c2, 0"
				:: "r"(0x08000029));

			memcpy((void *)ehdr->e_entry,
				(void *)(PATCH_ADDR + shdr->sh_offset),
				shdr->sh_size);

			// Drain write buffer
			__asm__ volatile ("mcr p15, 0, %0, c7, c10, 4" :: "r"(0));

			cur = ehdr->e_entry & ~(line - 1);
			btm = ehdr->e_entry + shdr->sh_size;
			while (cur < btm) {
				__asm__ volatile (
					// Clean Dcache
					"mcr p15, 0, %0, c7, c10, 1\n\t"

					// Flush Icache
					"mcr p15, 0, %0, c7, c5, 1"
					:: "r"(cur));

				cur += line;
			}

			((void (*)(uint32_t, void *, uintptr_t))ehdr->e_entry)(
				sector, keyxArg, 0x1FFFFFF8);

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
		decryptFirmKtrArm9((void *)FIRM_ADDR);
		uint8_t patch0[] = { 0x00, 0x20, 0x3B, 0xE0 };
        uint8_t patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
        memcpy((uint32_t*)(FIRM_ADDR + 0xB39D8), patch0, 4);
		memcpy((uint32_t*)(FIRM_ADDR + 0xB9204), patch1, 4);
	}

	return loadExecReboot();
}
