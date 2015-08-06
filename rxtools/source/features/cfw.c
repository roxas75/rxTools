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
#include "cfw.h"
#include "common.h"
#include "hid.h"
#include "console.h"
#include "aes.h"
#include "elf.h"
#include "sdmmc.h"
#include "fs.h"
#include "ncch.h"
#include "draw.h"
#include "menu.h"
#include "CTRDecryptor.h"
#include "TitleKeyDecrypt.h"
#include "configuration.h"

#define FIRM_ADDR (void*)0x24000000
#define ARMBXR4	0x47204C00
#define PLATFORM_REG ((volatile u32*)0x10140FFC)

char tmp[256];
char str[100];
unsigned int emuNandMounted = 0;
void (*_softreset)() = (void*)0x080F0000;

//----------- only for n3ds code --------------
#define DATAFOLDER	"rxtools/data"
#define KEYFILENAME	"slot0x25KeyX.bin"
#define WORKBUF		(u8*)0x21000000
#define NAT_SIZE	0xF1000
#define AGB_SIZE	0xD9C00
#define TWL_SIZE	0x1A1C00
#define PROGRESS_OK	L'?'
#define PROGRESS_FAIL	L'?'
#define PROGRESS_X	(SCREEN_WIDTH-7*FONT_WIDTH)/2

bool first_boot;
char tmpstr1[256] = { 0 };
char str[100];
char strl[100];
char strr[100];
File tempfile;
UINT tmpu32;

//------- end of "only for n3ds code" ---------


// @breif  Determine platform of the console.
// @retval PLATFORM_N3DS for New3DS, and PLATFORM_3DS for Old3DS.
// @note   Maybe modified to support more platforms
Platform_UnitType Platform_CheckUnit(void) {
	return *PLATFORM_REG;
}

static int loadExecReboot()
{
	File fd;

	if (!FileOpen(&fd, "/rxTools/system/reboot.bin", 0))
		return 1;

	if (FileRead(&fd, (void*)0x080F0000, 0x8000, 0) < 0)
		return 1;

	FileClose(&fd);
	_softreset();
	return 0;
}

static int firmlaunch(u8* firm){
	memcpy(FIRM_ADDR, firm, 0x200000); 	//Fixed size, no FIRM right now is that big
	return loadExecReboot();
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

void setFirmMode(int mode){ //0 : SysNand, 1 : EmuNand
	if(!checkEmuNAND()) mode = 0;	//forcing to SysNand if there is no EmuNand
	File firm;
	u32 mmc_original[] = { 0x000D0004, 0x001E0017 };
	u32 nat_emuwrite[] = { ARMBXR4, 0x0801A4C0 };
	u32 nat_emuread[] = { ARMBXR4, 0x0801A5B0 };
	if(FileOpen(&firm, "rxtools/data/0004013800000002.bin", 0)){
		FileWrite(&firm, mode ? &nat_emuwrite : &mmc_original, 8, 0xCCF2C);
		FileWrite(&firm, mode ? &nat_emuread : &mmc_original, 8, 0xCCF6C);
		FileWrite(&firm, mode ? rxmode_emu_label : rxmode_sys_label, 4, 0x7A5A0);
		FileClose(&firm);
	}
}

void setAgbBios()
{
	File agb_firm;
	unsigned char svc = (cfgs[CFG_AGB].val.i ? 0x26 : 0x01);
	if (FileOpen(&agb_firm, "rxtools/data/0004013800000202.bin", 0))
	{
		FileWrite(&agb_firm, &svc, 1, 0xD7A12);
		FileClose(&agb_firm);
	}
}

int rxMode(int mode){	//0 : SysNand, 1 : EmuNand

	if (Platform_CheckUnit() == PLATFORM_N3DS) //Patch at every boot, only for n3ds testing
	{
		DrawString(BOT_SCREEN, L"PREPARING RXMODE BOOT FOR N3DS", FONT_WIDTH, SCREEN_HEIGHT - FONT_HEIGHT, WHITE, BLACK);
		static const FirmInfo native_info = { 0x66A00, 0x8A600, 0x08006000, 0x33A00, 0x33000, 0x1FF80000, 0x15B00, 0x16700, 0x08028000 };

		char* drive = "0";
		//Create the workdir
		sprintf(tmpstr1, "%s:%s", drive, DATAFOLDER);
		f_mkdir(tmpstr1);

		FIL firmfile;

		//Read firmware data
		f_open(&firmfile, "firm.dat", FA_READ | FA_OPEN_EXISTING);

		//Create patched native_firm
		f_read(&firmfile, WORKBUF, NAT_SIZE, &tmpu32);
		u8* n_firm = WORKBUF;
		applyPatch(n_firm, "/rxTools/system/patches/native_firm.elf", &native_info);
		u8 keyx[16] = { 0 };
		if (GetSystemVersion() < 3){
			if (!FileOpen(&tempfile, KEYFILENAME, 0))
			{
				f_close(&firmfile);
			}
			FileRead(&tempfile, &keyx[0], 16, 0);
			FileClose(&tempfile);
		}
		DrawString(BOT_SCREEN, L"FIRM PATCHED", FONT_WIDTH, SCREEN_HEIGHT - FONT_HEIGHT, WHITE, BLACK);

		*((unsigned int*)(FIRM_ADDR + 0x7B0BC)) = checkEmuNAND() - 1;

		DrawString(BOT_SCREEN, L"aaaabbbb THING DONE", FONT_WIDTH, SCREEN_HEIGHT - FONT_HEIGHT, WHITE, BLACK);
		sprintf(tmpstr1, "%s:%s/0004013800000002.bin", drive, DATAFOLDER);
		if (FileOpen(&tempfile, tmpstr1, 1)){
			FileWrite(&tempfile, n_firm, NAT_SIZE, 0);
			FileClose(&tempfile);
		}
		else {
			f_close(&firmfile);
		}
		DrawString(BOT_SCREEN, L"READY TO BOOT", FONT_WIDTH, SCREEN_HEIGHT - FONT_HEIGHT, WHITE, BLACK);
	}

	//Boot
	setFirmMode(mode);
	if (Platform_CheckUnit() == PLATFORM_3DS) setAgbBios();

	File myFile;
	u8* native_firm = (u8*)0x21000000;
	if (FileOpen(&myFile, "rxtools/data/0004013800000002.bin", 0)){
		FileRead(&myFile, native_firm, 0xF0000, 0);
		FileClose(&myFile);
		firmlaunch(native_firm);
	}
	return -1;
}

void rxModeSys(){
	sprintf(str, "/rxTools/Theme/%u/boot.bin", cfgs[CFG_THEME].val.i);
	DrawBottomSplash(str);
	rxMode(0);
	sprintf(str, "/rxTools/Theme/%u/bootE.bin", cfgs[CFG_THEME].val.i);
	DrawBottomSplash(str);
	WaitForButton(BUTTON_A);
}

void rxModeEmu(){
	if (!checkEmuNAND()) rxModeSys();
	else{
		sprintf(str, "/rxTools/Theme/%u/boot.bin", cfgs[CFG_THEME].val.i);
		DrawBottomSplash(str);
		rxMode(1);
		sprintf(str, "/rxTools/Theme/%u/bootE.bin", cfgs[CFG_THEME].val.i);
		DrawBottomSplash(str);
		WaitForButton(BUTTON_A);
	}
}

void rxModeQuickBoot(){
	rxMode(1);
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
