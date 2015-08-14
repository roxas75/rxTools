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
#include "firm.h"
#include "mpcore.h"
#include "hid.h"
#include "lang.h"
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

const char firmPathFmt[] = "rxTools/data/00040138%08X.bin";

unsigned int emuNandMounted = 0;
_Noreturn void (* const _softreset)() = (void *)0x080F0000;

static FRESULT loadExecReboot()
{
	FIL fd;
	FRESULT r;
	UINT br;

	r = f_open(&fd, "/rxTools/system/reboot.bin", FA_READ);
	if (r != FR_OK)
		return r;

	r = f_read(&fd, (void*)0x080F0000, 0x8000, &br);
	if (r != FR_OK)
		return r;

	f_close(&fd);
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

	return *(uint32_t *)FIRM_ADDR == 0x4D524946 ? 0 : -1;
}

static unsigned int addrToOff(Elf32_Addr addr, const FirmInfo *info)
{
	if (addr >= info->arm9Entry && addr <= info->arm9Entry + info->p9Off)
		return info->arm9Off + (addr - info->arm9Entry);

	if (addr >= info->p9Entry && addr <= info->p9Entry + info->arm9Size - info->p9Start)
		return info->arm9Off + (addr - info->p9Entry) + info->p9Start;

	if (addr >= info->arm11Entry && addr <= info->arm11Entry + info->arm11Size)
	    return info->arm11Off + (addr - info->arm11Entry);

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

uint8_t* decryptFirmTitleNcch(uint8_t* title, unsigned int size){
	ctr_ncchheader NCCH;
	uint8_t CTR[16];
	PartitionInfo INFO;
	NCCH = *((ctr_ncchheader*)title);
	if(memcmp(NCCH.magic, "NCCH", 4) != 0) return NULL;
	ncch_get_counter(NCCH, CTR, 2);
	INFO.ctr = CTR; INFO.buffer = title + getle32(NCCH.exefsoffset)*0x200; INFO.keyY = NCCH.signature; INFO.size = size; INFO.keyslot = 0x2C;
	DecryptPartition(&INFO);
	uint8_t* firm = (uint8_t*)(INFO.buffer + 0x200);
	return firm;
}

uint8_t* decryptFirmTitle(uint8_t* title, unsigned int size, unsigned int tid, int drive){
	uint8_t key[0x10] = {0};
	uint8_t iv[0x10] = {0};
	GetTitleKey(&key[0], 0x00040138, tid, drive);
	aes_context aes_ctxt;
	aes_setkey_dec(&aes_ctxt, &key[0], 0x80);
	aes_crypt_cbc(&aes_ctxt, AES_DECRYPT, size, iv, title, title);
	return decryptFirmTitleNcch(title, size);
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

	static const FirmInfo ktrInfo = { 0x66A00, 0x8A600, 0x08006000, 0x33A00, 0x33000, 0x1FF80000, 0x15B00, 0x16700, 0x08028000 };
	static const FirmInfo ctrInfo = { 0x66000, 0x84A00, 0x08006800, 0x35000, 0x31000, 0x1FF80000, 0x15B00, 0x16700, 0x08028000 };
	static const char patchNandPrefix[] = ".patch.p9.nand";
	unsigned int cur, off, shstrSize;
	char path[64], shstrtab[512], *sh_name;
	const char *platformDir;
	const FirmInfo *info;
	const wchar_t *msg;
	int r, sector;
	void *p;
	Elf32_Ehdr ehdr;
	Elf32_Shdr shdr;
	FIL fd, keyxFd;
	UINT br;

	setAgbBios();

	getFirmPath(path, getMpInfo() == MPINFO_KTR ?
		TID_KTR_NATIVE_FIRM : TID_CTR_NATIVE_FIRM);
	r = loadFirm(path);
	if (r) {
		msg = L"Failed to load NATIVE_FIRM: %d\n"
			L"Reboot rxTools and try again.\n";
		goto fail;
	}

	r = getMpInfo();
	switch (r) {
		case MPINFO_KTR:
			info = &ktrInfo;
			platformDir = "ktr";
			break;

		case MPINFO_CTR:
			info = &ctrInfo;
			platformDir = "ctr";
			break;

		default:
			msg = L"Unknown Platform: %d";
			goto fail;
	}

	sprintf(path, "/rxTools/system/patches/%s/native_firm.elf", platformDir);
	r = f_open(&fd, path, FA_READ);
	if (r != FR_OK)
		goto patchFail;

	r = f_read(&fd, &ehdr, sizeof(ehdr), &br);
	if (r != FR_OK)
		goto patchFail;

	r = f_lseek(&fd, ehdr.e_shoff + ehdr.e_shstrndx * sizeof(Elf32_Shdr));
	if (r != FR_OK)
		goto patchFail;

	r = f_read(&fd, &shdr, sizeof(shdr), &br);
	if (r != FR_OK)
		goto patchFail;

	r = f_lseek(&fd, shdr.sh_offset);
	if (r != FR_OK)
		goto patchFail;

	r = f_read(&fd, shstrtab, shdr.sh_size > sizeof(shstrtab) ?
		sizeof(shstrtab) : shdr.sh_size, &shstrSize);
	if (r != FR_OK)
		goto patchFail;

	if (emu) {
		sector = checkEmuNAND();
		if (sector == 0) {
			msg = L"Failed to find EmuNAND.\n"
				L"Check your EmuNAND.\n";
			goto fail;
		}
	} else
		sector = 0;

	cur = ehdr.e_shoff;
	for (; ehdr.e_shnum; ehdr.e_shnum--, cur += sizeof(shdr)) {
		if (f_lseek(&fd, cur) != FR_OK)
			continue;

		if (f_read(&fd, &shdr, sizeof(shdr), &br) != FR_OK)
			continue;

		if (!(shdr.sh_flags & SHF_ALLOC) || shdr.sh_name >= shstrSize)
			continue;

		off = addrToOff(shdr.sh_addr, info);
		if (off == 0)
			continue;

		p = (void *)(FIRM_ADDR + off);
		sh_name = shstrtab + shdr.sh_name;

		if (!strcmp(sh_name, ".rodata.keyx")) {
			if (sysver >= 7)
				continue;

			if (f_open(&keyxFd, "slot0x25KeyX.bin", FA_READ) != FR_OK)
				continue;

			f_read(&keyxFd, p, shdr.sh_size, &br);
			f_close(&keyxFd);
		} else if (!strcmp(sh_name, ".rodata.nand.sector")) {
			if (sector)
				*(uint32_t *)p = (sector / 0x200) - 1;
		} else if (!strcmp(sh_name, ".rodata.label")) {
			memcpy(p, "RX-", 3);
			((char *)p)[3] = sector ? 'E' : 'S';
		} else if (shdr.sh_type == SHT_PROGBITS
			&& (sector || memcmp(sh_name, patchNandPrefix, sizeof(patchNandPrefix) - 1))
			&& (sysver < 7 || strcmp(sh_name, ".patch.p9.keyx")))
		{
			if (f_lseek(&fd, shdr.sh_offset) != FR_OK)
				continue;

			f_read(&fd, p, shdr.sh_size, &br);
		}
	}

	f_open(&fd, "NATIVE_FIRM.BIN", FA_WRITE | FA_CREATE_ALWAYS);
	f_write(&fd, (void *)FIRM_ADDR, 0x200000, &br);
	f_close(&fd);

	r = loadExecReboot(); // This won't return if it succeeds.
	msg = L"Failed to load reboot.bin: %d\n"
		L"Check your installation.\n";

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
	msg = L"Failed to load native_firm.elf: %d\n"
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
int DevMode(){
	/*DevMode is ready for n3ds BUT there's an unresolved bug which affects nand reading functions, like nand_readsectors(0, 0xF0000 / 0x200, firm, FIRM0);*/

	uint8_t* firm = (void*)0x24000000;
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
