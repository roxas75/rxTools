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
#include "cfw.h"
#include "common.h"
#include "hid.h"
#include "console.h"
#include "polarssl/aes.h"
#include "elf.h"
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

#define FIRM_ADDR 0x24000000
#define ARMBXR4	0x47204C00
#define PLATFORM_REG_ADDR 0x10140FFC

unsigned int emuNandMounted = 0;
_Noreturn void (* const _softreset)() = (void *)0x080F0000;

// @breif  Determine platform of the console.
// @retval PLATFORM_N3DS for New3DS, and PLATFORM_3DS for Old3DS.
// @note   Maybe modified to support more platforms
static Platform_UnitType Platform_CheckUnit(void) {
	return *(u32 *)PLATFORM_REG_ADDR;
}

static int loadExecReboot()
{
	File fd;

	if (!FileOpen(&fd, "/rxTools/system/reboot.bin", 0))
		return -1;

	if (FileRead(&fd, (void*)0x080F0000, 0x8000, 0) < 0)
		return -1;

	FileClose(&fd);
	_softreset();
}

static int loadFirm(char *path)
{
	FIL fd;
	FRESULT r;
	UINT br;

	r = f_open(&fd, path, FA_READ);
	if (r != FR_OK)
		return r;

	r = f_read(&fd, (void *)FIRM_ADDR, 0x200000, &br);
	if (r != FR_OK)
		return r;

	f_close(&fd);

	return *(u32 *)FIRM_ADDR == 0x4D524946 ? 0 : -1;
}

static unsigned int addrToOff(Elf32_Addr addr, const FirmInfo *info)
{
	if (addr >= info->arm9Entry && addr <= info->arm9Entry + info->p9Off)
		return info->arm9Off + (addr - info->arm9Entry);

	if (addr >= info->p9Entry && addr <= info->p9Entry + info->arm9Size - info->p9Start)
		return info->arm9Off + (addr - info->p9Entry) + info->p9Start;

	return 0;
}

int applyPatch(void *file, const char *patch, const FirmInfo *info)
{
	File fd;
	Elf32_Ehdr ehdr;
	Elf32_Shdr shdr;
	unsigned int cur, off;

	if (!FileOpen(&fd, patch, 0))
		return 1;

	if (FileRead(&fd, &ehdr, sizeof(ehdr), 0) < 0)
		return 1;

	cur = ehdr.e_shoff;
	for (; ehdr.e_shnum; ehdr.e_shnum--, cur += sizeof(shdr)) {
		if (FileRead(&fd, &shdr, sizeof(shdr), cur) < 0)
			continue;

		if (shdr.sh_type != SHT_PROGBITS || !(shdr.sh_flags & SHF_ALLOC))
			continue;

		off = addrToOff(shdr.sh_addr, info);
		if (off == 0)
			continue;

		FileRead(&fd, (void *)((uintptr_t)file + off), shdr.sh_size, shdr.sh_offset);
	}

	return 0;
}

u8* decryptFirmTitleNcch(u8* title, unsigned int size){
	ctr_ncchheader NCCH;
	u8 CTR[16];
	PartitionInfo INFO;
	NCCH = *((ctr_ncchheader*)title);
	if(memcmp(NCCH.magic, "NCCH", 4) != 0) return NULL;
	ncch_get_counter(NCCH, CTR, 2);
	INFO.ctr = CTR; INFO.buffer = title + getle32(NCCH.exefsoffset)*0x200; INFO.keyY = NCCH.signature; INFO.size = size; INFO.keyslot = 0x2C;
	DecryptPartition(&INFO);
	u8* firm = (u8*)(INFO.buffer + 0x200);
	return firm;
}

u8* decryptFirmTitle(u8* title, unsigned int size, unsigned int tid, int drive){
	u8 key[0x10] = {0};
	u8 iv[0x10] = {0};
	GetTitleKey(&key[0], 0x00040138, tid, drive);
	aes_context aes_ctxt;
	aes_setkey_dec(&aes_ctxt, &key[0], 0x80);
	aes_crypt_cbc(&aes_ctxt, AES_DECRYPT, size, iv, title, title);
	return decryptFirmTitleNcch(title, size);
}

static void setAgbBios()
{
	File agb_firm;
	unsigned char svc = (cfgs[CFG_AGB].val.i ? 0x26 : 0x01);
	if (FileOpen(&agb_firm, "rxtools/data/0004013800000202.bin", 0))
	{
		FileWrite(&agb_firm, &svc, 1, 0xD7A12);
		FileClose(&agb_firm);
	}
}

int rxMode(int emu)
{
	if (!checkEmuNAND() && emu)
	{
		ConsoleInit();
		ConsoleSetTitle(L"EMUNAND NOT FOUND!");
		print(L"The emunand was not found on\n");
		print(L"your SDCard. \n");
		print(L"\n");
		print(L"Press A to boot SYSNAND\n");
		ConsoleShow();
		WaitForButton(BUTTON_A);
		emu = 0;
		char s[32];
		sprintf(s, "/rxTools/Theme/%u/boot.bin", cfgs[CFG_THEME].val.i);
		DrawBottomSplash(s);
	}

	static const FirmInfo info = { 0x66000, 0x84A00, 0x08006800, 0x15B00, 0x16700, 0x08028000 };
	static const char patchNandPrefix[] = ".patch.p9.nand";
	unsigned int cur, off, shstrSize;
	char shstrtab[512], *sh_name;
	int r, sector;
	void *p;
	Elf32_Ehdr ehdr;
	Elf32_Shdr shdr;
	FIL fd, keyxFd;
	UINT br;

	setAgbBios();

	r = loadFirm("rxtools/data/0004013800000002.bin");
	if (r)
		return r;

	r = f_open(&fd, "/rxTools/system/patches/native_firm.elf", FA_READ);
	if (r != FR_OK)
		return r;

	r = f_read(&fd, &ehdr, sizeof(ehdr), &br);
	if (r != FR_OK)
		return r;

	r = f_lseek(&fd, ehdr.e_shoff + ehdr.e_shstrndx * sizeof(Elf32_Shdr));
	if (r != FR_OK)
		return r;

	r = f_read(&fd, &shdr, sizeof(shdr), &br);
	if (r != FR_OK)
		return r;

	r = f_lseek(&fd, shdr.sh_offset);
	if (r != FR_OK)
		return r;

	r = f_read(&fd, shstrtab, shdr.sh_size > sizeof(shstrtab) ?
		sizeof(shstrtab) : shdr.sh_size, &shstrSize);
	if (r != FR_OK)
		return r;

	sector = emu ? checkEmuNAND() : 0;

	cur = ehdr.e_shoff;
	for (; ehdr.e_shnum; ehdr.e_shnum--, cur += sizeof(shdr)) {
		if (f_lseek(&fd, cur) != FR_OK)
			continue;

		if (f_read(&fd, &shdr, sizeof(shdr), &br) != FR_OK)
			continue;

		if (!(shdr.sh_flags & SHF_ALLOC) || shdr.sh_name >= shstrSize)
			continue;

		off = addrToOff(shdr.sh_addr, &info);
		if (off == 0)
			continue;

		p = (void *)(FIRM_ADDR + off);
		sh_name = shstrtab + shdr.sh_name;

		if (!strcmp(sh_name, ".rodata.keyx")) {
			if (f_open(&keyxFd, "slot0x25KeyX.bin", FA_READ) != FR_OK)
				continue;

			f_read(&keyxFd, p, shdr.sh_size, &br);
			f_close(&keyxFd);
		} else if (!strcmp(sh_name, ".rodata.nand.sector")) {
			if (sector)
				*(u32 *)p = (sector / 0x200) - 1;
		} else if (!strcmp(sh_name, ".rodata.label")) {
			memcpy(p, "RX-", 3);
			((char *)p)[3] = sector ? 'E' : 'S';
		} else if (shdr.sh_type == SHT_PROGBITS
			&& (sector || memcmp(sh_name, patchNandPrefix, sizeof(patchNandPrefix) - 1)))
		{
			if (f_lseek(&fd, shdr.sh_offset) != FR_OK)
				continue;

			f_read(&fd, p, shdr.sh_size, &br);
		}
	}

	return loadExecReboot();
}

void rxModeWithSplash(int emu)
{
	char s[32];

	sprintf(s, "/rxTools/Theme/%u/boot.bin", cfgs[CFG_THEME].val.i);
	DrawBottomSplash(s);
	rxMode(emu);
	sprintf(s, "/rxTools/Theme/%u/bootE.bin", cfgs[CFG_THEME].val.i);
	DrawBottomSplash(s);
	WaitForButton(BUTTON_A);
}

//Just patches signatures check, loads in sysnand
int DevMode(){
	/*DevMode is ready for n3ds BUT there's an unresolved bug which affects nand reading functions, like nand_readsectors(0, 0xF0000 / 0x200, firm, FIRM0);*/

	u8* firm = (void*)0x24000000;
	nand_readsectors(0, 0xF0000 / 0x200, firm, FIRM0);
	if (strncmp((char*)firm, "FIRM", 4))
		nand_readsectors(0, 0xF0000 / 0x200, firm, FIRM1);

	if(Platform_CheckUnit() == PLATFORM_3DS)
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
		u8 patch1[] = { 0x6D, 0x20, 0xCE, 0x77 };
		u8 patch2[] = { 0x5A, 0xC5, 0x73, 0xC1 };
		memcpy((u32*)0x08052FD8, patch1, 4);
		memcpy((u32*)0x08058804, patch2, 4);
	}

	return loadExecReboot();
}

void FirmLoader(){
	char firm_path[256];

	if (!FileExplorerMain(firm_path, sizeof(firm_path)))
	{
		if (loadFirm(firm_path))
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
