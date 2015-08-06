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

#include <wchar.h>
#include "NandDumper.h"
#include "common.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "fs.h"
#include "screenshot.h"
#include "padgen.h"
#include "crypto.h"
#include "ncch.h"
#include "CTRDecryptor.h"
#include "fatfs/sdmmc.h"
#include "stdio.h"

#define NAND_SIZE 0x3AF00000
#define NAND_SECTOR_SIZE 0x200
#define BUF1 (void*)0x21000000
#define PROGRESS_OK	L"⬛"

int NandSwitch(){
	if(!checkEmuNAND()) return  0; //If No EmuNAND, we force to work on SysNAND
	ConsoleInit();
	/* freezes
	ConsoleSetTitle(STR_CHOOSE_NAND[language]);
	print(STR_PRESS_X_SYSNAND[language]);
	print(STR_PRESS_Y_EMUNAND[language]);
	print(STR_PRESS_B_BACK[language]);
	*/
	ConsoleSetTitle(L"Choose the NAND you want to use");
	print(L"Ⓧ sysNAND\nⓎ emuNAND\nⒷ Cancel\n");
	ConsoleShow();
	while (true) {
        u32 pad_state = InputWait();
		if(pad_state & BUTTON_X) return SYS_NAND;
		if(pad_state & BUTTON_Y) return EMU_NAND;
		if(pad_state & BUTTON_B) return UNK_NAND;
    }
}

void NandDumper(){
	File myFile;
	int isEmuNand = SYS_NAND;
	if(checkEmuNAND() && (isEmuNand = NandSwitch()) == UNK_NAND) return;
	isEmuNand--;
	ConsoleInit();
	ConsoleSetTitle(L"%sNAND Dumper", isEmuNand ? "emu" : "sys");
	unsigned char* buf = (void*)0x21000000;
	unsigned int nsectors = 0x200;  //sectors in a row
	wchar_t ProgressBar[] = L"⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜ ";
	unsigned int progress = 0;
/*      int BACKCOLOR = */ConsoleGetBackgroundColor(); //can be removed, left only to keep binaries the same
	if(FileOpen(&myFile, isEmuNand ? "rxTools/nand/EMUNAND.bin" : "rxTools/nand/NAND.bin", 1)){
		print(L"Dumping...\n\n");
		ConsoleShow();
		int x, y;
		ConsoleGetXY(&x, &y);
		y += FONT_HEIGHT * 6;
		x += FONT_HWIDTH * 2;
		DrawString(BOT_SCREEN, ProgressBar, x, y, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
		DrawString(BOT_SCREEN, L"Press Ⓑ anytime to abort", x, y + FONT_HEIGHT*2, ConsoleGetTextColor(), ConsoleGetBackgroundColor());

		for(int count = 0; count < NAND_SIZE/NAND_SECTOR_SIZE/nsectors; count++){

			if(isEmuNand) sdmmc_sdcard_readsectors(count*nsectors, nsectors, buf);
			else sdmmc_nand_readsectors(count*nsectors, nsectors, buf);

			FileWrite(&myFile, buf, nsectors*NAND_SECTOR_SIZE, count*NAND_SECTOR_SIZE*nsectors);
			TryScreenShot();
			if((count % (int)(NAND_SIZE/NAND_SECTOR_SIZE/nsectors/16)) == 0 && count != 0){
				DrawString(BOT_SCREEN, PROGRESS_OK, x+(FONT_WIDTH*(progress++)), y, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
			}
			unsigned int pad = GetInput();
			if(pad & BUTTON_B) break;
		}
		if(isEmuNand){
			sdmmc_sdcard_readsectors(checkEmuNAND()/0x200, 1, buf);
			FileWrite(&myFile, buf, 0x200, 0);
		}
		FileClose(&myFile);
		print(L"\nFinished dumping!\n");
		ConsoleShow();
	}else{
		print(L"Failed to create the dump.\n");
		ConsoleShow();
	}
	print(L"\nPress Ⓐ to exit\n");
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

void DumpNandPartitions(){
	int isEmuNand = SYS_NAND;
	if(checkEmuNAND() && (isEmuNand = NandSwitch()) == UNK_NAND) return;
	isEmuNand--;
	char* p_name[] = { "twln.bin", "twlp.bin", "agb_save.bin", "firm0.bin", "firm1.bin", "ctrnand.bin" };
	unsigned int p_size[] = { 0x08FB5200, 0x020B6600, 0x00030000, 0x00400000, 0x00400000, 0x2F3E3600};
	unsigned int p_addr[] = { TWLN, TWLP, AGB_SAVE, FIRM0, FIRM1, CTRNAND };
	int sect_row = 0x80;

	ConsoleInit();
	ConsoleSetTitle(L"%sNAND Partitions Decryptor", isEmuNand ? "emu" : "sys");

	char tmp[256];
	wchar_t wtmp[256];
	for(int i = 3; i < 6; i++){		//Cutting out twln, twlp and agb_save. Todo: Properly decrypt them
		File out;
		sprintf(tmp, "rxTools/nand/%s%s", isEmuNand ? "emu_" : "", p_name[i]);
		FileOpen(&out, tmp, 1);
		print(L"Dumping %s ...\n", p_name[i]);
		ConsoleShow();

		for(int j = 0; j*0x200 < p_size[i]; j += sect_row){
			swprintf(wtmp, sizeof(wtmp)/sizeof(wtmp[0]), L"%08X / %08X", j*0x200, p_size[i]);
			int x, y;
			ConsoleGetXY(&x, &y);
			y += FONT_HEIGHT * 3;
			x += FONT_HWIDTH*2;
			DrawString(BOT_SCREEN, wtmp, x, y, ConsoleGetTextColor(), ConsoleGetBackgroundColor());

			if(isEmuNand) emunand_readsectors(j, sect_row, BUF1, p_addr[i]);
			else nand_readsectors(j, sect_row, BUF1, p_addr[i]);
			FileWrite(&out, BUF1, sect_row*0x200, j*0x200);
		}
		FileClose(&out);
	}
	print(L"\nPress Ⓐ to exit\n");
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

void GenerateNandXorpads(){
	PadInfo myInfo = {.keyslot = 0x4, .setKeyY = 0, .size_mb = 760, .filename = "rxTools/nand.fat16.xorpad"};
	GetNANDCTR(myInfo.CTR); add_ctr(myInfo.CTR, 0xB93000);

	ConsoleInit();
	ConsoleSetTitle(L"NAND Xorpad Generator");
	print(L"Generating nand.fat16.xorpad ...\n");
	ConsoleShow();
	CreatePad(&myInfo, 0);

	print(L"\nPress Ⓐ to exit\n");
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

void DumpNANDSystemTitles(){
	int isEmuNand = SYS_NAND;
	if(checkEmuNAND() && (isEmuNand = NandSwitch()) == UNK_NAND) return;
	isEmuNand--;
	char* outfolder = "rxTools/titles";

	ConsoleInit();
	ConsoleSetTitle(L"%sNAND System Titles Dumper", isEmuNand ? "emu" : "sys");
	print(L"This should take long, the\nentire ctrnand will be scanned.\nBe patient and wait until\nthe research is finished.\n");
	ConsoleShow();

	File pfile;
	char filename[256];
	int nTitle = 0;
	unsigned int tot_size = 0x179000;
	f_mkdir (outfolder);
	for(int i = 0; i < tot_size; i++){
		if(isEmuNand) emunand_readsectors(i, 1, BUF1, CTRNAND);
		else nand_readsectors(i, 1, BUF1, CTRNAND);
		if(*((char*)BUF1 + 0x100) == 'N' && *((char*)BUF1 + 0x101) == 'C' && *((char*)BUF1 + 0x102) == 'C' && *((char*)BUF1 + 0x103) == 'H'){
			ctr_ncchheader ncch;
			memcpy((void*)&ncch, BUF1, 0x200);
			sprintf(filename, "%s/%s%08X%08X.app", isEmuNand ? "emu_" : "", outfolder, *((unsigned int*)(BUF1 + 0x10C)), *((unsigned int*)(BUF1 + 0x108)));
			ConsoleInit();
			print(L"%sNAND System Titles Dumper\n\nDumping...%s\n", isEmuNand ? "emu" : "sys", (char*)filename + strlen(outfolder) + 1);
			ConsoleShow();
			FileOpen(&pfile, filename, 1);
			for(int j = 0; j < getle32(ncch.contentsize); j++){
				if(isEmuNand) emunand_readsectors(i + j, 1, BUF1, CTRNAND);
				else nand_readsectors(i + j, 1, BUF1, CTRNAND);
				FileWrite(&pfile, BUF1, 0x200, j*0x200);
			}
			FileClose(&pfile);
			i += getle32(ncch.contentsize);
			nTitle++;
		}
	}
	ConsoleInit();
	print(L"The entire ctrnand has been\nscanned and %d CTR titles have\nbeen dumped.\nDo you want to decrypt them\nall with CTRDecryptor?\n\nPress Ⓐ : Decrypt titles\nPress Ⓑ : Exit\n", nTitle);
	ConsoleShow();
	while (true) {
        u32 pad_state = InputWait();
		if(pad_state & BUTTON_A){
			CTRDecryptor();
			break;
		}
		if(pad_state & BUTTON_B) 	break;
    }
}

void RebuildNand(){
	char* p_name[] = { "twln.bin", "twlp.bin", "agb_save.bin", "firm0.bin", "firm1.bin", "ctrnand.bin" };
	unsigned int p_size[] = { 0x08FB5200, 0x020B6600, 0x00030000, 0x00400000, 0x00400000, 0x2F3E3600};
	unsigned int p_addr[] = { TWLN, TWLP, AGB_SAVE, FIRM0, FIRM1, CTRNAND };
	int sect_row = 0x1;			//Slow, ok, but secure

	ConsoleInit();
	int isEmuNand = checkEmuNAND();
	ConsoleSetTitle(L"%sNAND Partitions Injector", isEmuNand ? "emu" : "sys");
	if(!isEmuNand){
		print(L"No EmuNAND mounted.\n\nPress Ⓐ to exit\n");
		ConsoleShow();
		WaitForButton(BUTTON_A);
		return;
	}
	char tmp[256];
	wchar_t wtmp[256];
	for(int i = 3; i < 6; i++){		//Cutting out twln, twlp and agb_save. Todo: Properly decrypt them
		File out;
		sprintf(tmp, "rxTools/nand/%s%s", isEmuNand ? "emu_" : "", p_name[i]);
		if(FileOpen(&out, tmp, 0)){
			print(L"Injecting %s%s\n", isEmuNand ? "emu_" : "", p_name[i]);
			ConsoleShow();

			for(int j = 0; j*0x200 < p_size[i]; j += sect_row){
				swprintf(wtmp, sizeof(wtmp)/sizeof(wtmp[0]), L"%08X / %08X", j*0x200, p_size[i]);
				int x, y;
				ConsoleGetXY(&x, &y);
				y += FONT_HEIGHT * 3;
				x += FONT_HWIDTH*2;
				DrawString(BOT_SCREEN, wtmp, x, y, ConsoleGetTextColor(), ConsoleGetBackgroundColor());

				FileRead(&out, BUF1, sect_row*0x200, j*0x200);
				if(isEmuNand) emunand_writesectors(j, sect_row, BUF1, p_addr[i]);
			}
			FileClose(&out);
		}
	}
	print(L"\nPress Ⓐ to exit\n");
	ConsoleShow();
	WaitForButton(BUTTON_A);
}
