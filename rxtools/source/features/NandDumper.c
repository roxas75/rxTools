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
#include <wchar.h>
#include "NandDumper.h"
#include "console.h"
#include "draw.h"
#include "lang.h"
#include "hid.h"
#include "fs.h"
#include "screenshot.h"
#include "padgen.h"
#include "crypto.h"
#include "mpcore.h"
#include "ncch.h"
#include "CTRDecryptor.h"
#include "fatfs/sdmmc.h"
#include "stdio.h"

#define NAND_SIZE getMpInfo() == MPINFO_KTR ? 0x4D800000 : 0x3AF00000
#define NAND_SECTOR_SIZE 0x200
#define BUF1 (void*)0x21000000
#define PROGRESS_WIDTH	16

int NandSwitch(){
	if(!checkEmuNAND()) return  0; //If No EmuNAND, we force to work on SysNAND
	ConsoleInit();
	print(strings[STR_CHOOSE], strings[STR_NAND]);
	print(strings[STR_BLANK_BUTTON_ACTION], strings[STR_BUTTON_X], strings[STR_SYSNAND]);
	print(strings[STR_BLANK_BUTTON_ACTION], strings[STR_BUTTON_Y], strings[STR_EMUNAND]);
	print(strings[STR_BLANK_BUTTON_ACTION], strings[STR_BUTTON_B], strings[STR_CANCEL]);

	ConsoleShow();
	while (true) {
        uint32_t pad_state = InputWait();
		if(pad_state & BUTTON_X) return SYS_NAND;
		if(pad_state & BUTTON_Y) return EMU_NAND;
		if(pad_state & BUTTON_B) return UNK_NAND;
    }
}

void NandDumper(){
	ConsoleSetTitle(strings[STR_DUMP], strings[STR_NAND]);
	File myFile;
	int isEmuNand = SYS_NAND;
	if(checkEmuNAND() && (isEmuNand = NandSwitch()) == UNK_NAND) return;
	isEmuNand--;
	ConsoleInit();
	ConsoleSetTitle(strings[STR_DUMP], strings[STR_NAND]);
	unsigned char* buf = (void*)0x21000000;
	unsigned int nsectors = 0x200;  //sectors in a row
	wchar_t tmpstr[STR_MAX_LEN];
	wchar_t ProgressBar[41] = {0,};
	for(int i=0; i<PROGRESS_WIDTH; i++)
		wcscat(ProgressBar, strings[STR_PROGRESS]);
	unsigned int progress = 0;
	char filename[256];
	sprintf(filename, "rxTools/nand/%sNAND.bin", isEmuNand ? "EMU" : "");
	if(FileOpen(&myFile, filename, 1)){
		print(strings[STR_DUMPING], isEmuNand ? strings[STR_EMUNAND] : strings[STR_SYSNAND], filename);
		ConsoleShow();
		int x, y;
		ConsoleGetXY(&x, &y);
		y += FONT_HEIGHT * 6;
		x += FONT_HWIDTH * 2;

		DrawString(BOT_SCREEN, ProgressBar, x, y, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
		swprintf(tmpstr, STR_MAX_LEN, strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_B], strings[STR_CANCEL]);
		DrawString(BOT_SCREEN, tmpstr, x, y + FONT_HEIGHT*2, ConsoleGetTextColor(), ConsoleGetBackgroundColor());

		for(int count = 0; count < NAND_SIZE/NAND_SECTOR_SIZE/nsectors; count++){

			if(isEmuNand) sdmmc_sdcard_readsectors(count*nsectors, nsectors, buf);
			else sdmmc_nand_readsectors(count*nsectors, nsectors, buf);

			FileWrite(&myFile, buf, nsectors*NAND_SECTOR_SIZE, count*NAND_SECTOR_SIZE*nsectors);
			TryScreenShot();
			if((count % (int)(NAND_SIZE/NAND_SECTOR_SIZE/nsectors/PROGRESS_WIDTH)) == 0 && count != 0){
				DrawString(BOT_SCREEN, strings[STR_PROGRESS_OK], x+(FONT_WIDTH*(progress++)), y, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
			}
			unsigned int pad = GetInput();
			if(pad & BUTTON_B) break;
		}
		if(isEmuNand){
			sdmmc_sdcard_readsectors(checkEmuNAND()/0x200, 1, buf);
			FileWrite(&myFile, buf, 0x200, 0);
		}
		FileClose(&myFile);
		print(strings[STR_COMPLETED]);
		ConsoleShow();
	}else{
		print(strings[STR_FAILED]);
		ConsoleShow();
	}
	print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

void DumpNandPartitions(){
	ConsoleSetTitle(strings[STR_DUMP], strings[STR_NAND_PARTITIONS]);
	int isEmuNand = SYS_NAND;
	if(checkEmuNAND() && (isEmuNand = NandSwitch()) == UNK_NAND) return;
	isEmuNand--;
	ConsoleInit();
	ConsoleSetTitle(strings[STR_DUMP], strings[STR_NAND_PARTITIONS]);
	print(strings[STR_PROCESSING], isEmuNand ? strings[STR_EMUNAND] : strings[STR_SYSNAND]);
	char* p_name[] = { "twln.bin", "twlp.bin", "agb_save.bin", "firm0.bin", "firm1.bin", "ctrnand.bin" };
	wchar_t* p_descr[] = { strings[STR_TWLN], strings[STR_TWLP], strings[STR_AGB_SAVE], strings[STR_FIRM0], strings[STR_FIRM1], strings[STR_CTRNAND] };
	unsigned int p_size[] = { 0x08FB5200, 0x020B6600, 0x00030000, 0x00400000, 0x00400000, getMpInfo() == MPINFO_KTR ? 0x41D2D200 : 0x2F3E3600 };
	unsigned int p_addr[] = { TWLN, TWLP, AGB_SAVE, FIRM0, FIRM1, CTRNAND };
	int sect_row = 0x80;

	char tmp[256];
	wchar_t wtmp[256];
	for(int i = 3; i < 6; i++){		//Cutting out twln, twlp and agb_save. Todo: Properly decrypt them
		File out;
		sprintf(tmp, "rxTools/nand/%s%s", isEmuNand ? "emu_" : "", p_name[i]);
		FileOpen(&out, tmp, 1);
		print(strings[STR_DUMPING], p_descr[i], tmp);
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
	print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

void GenerateNandXorpads(){

	PadInfo myInfo = { .keyslot = getMpInfo() == MPINFO_KTR ? 0x5 : 0x4, .setKeyY = 0, .size_mb = getMpInfo() == MPINFO_KTR ? 1055 : 758, .filename = "rxTools/nand.fat16.xorpad" };
	GetNANDCTR(myInfo.CTR); add_ctr(myInfo.CTR, 0xB93000);

	ConsoleInit();
	ConsoleSetTitle(strings[STR_GENERATE], strings[STR_NAND_XORPAD]);
	print(strings[STR_DUMPING], strings[STR_NAND_XORPAD], myInfo.filename);
	ConsoleShow();
	CreatePad(&myInfo, 0);

	print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

void DumpNANDSystemTitles(){
	ConsoleSetTitle(strings[STR_DUMP], strings[STR_SYSTEM_TITLES]);
	int isEmuNand = SYS_NAND;
	if(checkEmuNAND() && (isEmuNand = NandSwitch()) == UNK_NAND) return;
	isEmuNand--;
	ConsoleInit();
	ConsoleSetTitle(strings[STR_DUMP], strings[STR_SYSTEM_TITLES]);
	print(strings[STR_PROCESSING], isEmuNand ? strings[STR_EMUNAND] : strings[STR_SYSNAND]);
	char* outfolder = "rxTools/titles";
	print(strings[STR_SYSTEM_TITLES_WARNING]);
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
			sprintf(filename, "%s/%s%08X%08X.app", outfolder, isEmuNand ? "emu_" : "", *((unsigned int*)(BUF1 + 0x10C)), *((unsigned int*)(BUF1 + 0x108)));
			ConsoleInit();
			print(strings[STR_DUMPING], L"", filename);
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
	print(strings[STR_SYSTEM_TITLES_DECRYPT], nTitle);
	print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
	print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_B], strings[STR_CANCEL]);
	ConsoleShow();
	while (true) {
        uint32_t pad_state = InputWait();
		if(pad_state & BUTTON_A){
			CTRDecryptor();
			break;
		}
		if(pad_state & BUTTON_B) 	break;
    }
}

void RebuildNand(){
	char* p_name[] = { "twln.bin", "twlp.bin", "agb_save.bin", "firm0.bin", "firm1.bin", "ctrnand.bin" };
	wchar_t* p_descr[] = { strings[STR_TWLN], strings[STR_TWLP], strings[STR_AGB_SAVE], strings[STR_FIRM0], strings[STR_FIRM1], strings[STR_CTRNAND] };
	unsigned int p_size[] = { 0x08FB5200, 0x020B6600, 0x00030000, 0x00400000, 0x00400000, getMpInfo() == MPINFO_KTR ? 0x41D2D200 : 0x2F3E3600 };
	unsigned int p_addr[] = { TWLN, TWLP, AGB_SAVE, FIRM0, FIRM1, CTRNAND };
	int sect_row = 0x1;			//Slow, ok, but secure

	ConsoleInit();
	int isEmuNand = checkEmuNAND();
	ConsoleSetTitle(strings[STR_INJECT], strings[STR_NAND_PARTITIONS]);
	if(!isEmuNand){
		print(strings[STR_NO_EMUNAND]);
		print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CANCEL]);
		ConsoleShow();
		WaitForButton(BUTTON_A);
		return;
	}
	print(strings[STR_PROCESSING], isEmuNand ? strings[STR_EMUNAND] : strings[STR_SYSNAND]);
	char tmp[256];
	wchar_t wtmp[256];
	for(int i = 3; i < 6; i++){		//Cutting out twln, twlp and agb_save. Todo: Properly decrypt them
		File out;
		sprintf(tmp, "rxTools/nand/%s%s", isEmuNand ? "emu_" : "", p_name[i]);
		if(FileOpen(&out, tmp, 0)){
			print(strings[STR_INJECTING], tmp, p_descr[i]);
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
	print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
	ConsoleShow();
	WaitForButton(BUTTON_A);
}
