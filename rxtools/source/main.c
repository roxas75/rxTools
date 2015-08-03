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
#include <wchar.h>
#include "common.h"
#include "MainMenu.h"
#include "crypto.h"
#include "fs.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "screenshot.h"
#include "filepack.h"
#include "cfw.h"
#include "configuration.h"

#define FONT_ADDRESS	(void*)0x27E00000
extern unsigned char *fontaddr;

void LoadFont(){
	File MyFile;
	if (FileOpen(&MyFile, "/rxTools/font.bin", 0))
	{
		FileRead(&MyFile, FONT_ADDRESS, 0x200000, 0);
		fontaddr = FONT_ADDRESS;
	}else{
		DrawString(BOT_SCREEN, L"Font load error", FONT_WIDTH, SCREEN_HEIGHT-FONT_HEIGHT, RED, BLACK);
	}
}

void Initialize(){
	char str[100];
	char strl[100];
	char strr[100];
	DrawString(BOT_SCREEN, L"INITIALIZE...", FONT_WIDTH, SCREEN_HEIGHT-FONT_HEIGHT, WHITE, BLACK);
	if(FSInit()){
		DrawString(BOT_SCREEN, L"LOADING...   ", FONT_WIDTH, SCREEN_HEIGHT-FONT_HEIGHT, WHITE, BLACK);
	}else{
		DrawString(BOT_SCREEN, L"ERROR!       ", FONT_WIDTH, SCREEN_HEIGHT-FONT_HEIGHT, RED, BLACK);
	}
	LoadFont();
	LoadPack();

	//Console Stuff
	ConsoleSetXY(15, 20);
	ConsoleSetWH(SCREEN_WIDTH-30, SCREEN_HEIGHT-80);
	ConsoleSetBorderColor(BLUE);
	ConsoleSetTextColor(RGB(0, 141, 197));
	ConsoleSetBackgroundColor(TRANSPARENT);
	ConsoleSetSpecialColor(BLUE);
	ConsoleSetSpacing(2);
	ConsoleSetBorderWidth(3);
	//Check that the data is installed
	f_mkdir("rxTools");
	f_mkdir("rxTools/nand");
	InstallConfigData();
	readCfg();
	loadStrings();

	sprintf(str, "/rxTools/Theme/%u/TOP.bin", cfgs[CFG_THEME].val.i);
	sprintf(strl, "/rxTools/Theme/%u/TOPL.bin", cfgs[CFG_THEME].val.i);
	sprintf(strr, "/rxTools/Theme/%u/TOPR.bin", cfgs[CFG_THEME].val.i);
	if (cfgs[CFG_3D].val.i)
		DrawTopSplash(str, strl, strr);
	else
		DrawTopSplash(str, str, str);

	if (!cfgs[CFG_GUI].val.i)
	{
		if(cfgs[CFG_SILENT].val.i)
		{
			sprintf(str, "/rxTools/Theme/%u/boot.bin", cfgs[CFG_THEME].val.i);
			DrawBottomSplash(str);

			for (int i = 0; i < 0x333333 * 2; i++){
				u32 pad = GetInput();
				if (pad & BUTTON_R1 && i > 0x333333) goto rxTools_boot;
			}
		}
		else
		{
			ConsoleInit();
			ConsoleSetTitle(L"%24ls",strings[STR_AUTOBOOT]);
			print(strings[STR_HOLD_R]);
			ConsoleShow();

			for (int i = 0; i < 0x333333 * 6; i++){
				u32 pad = GetInput();
				if (pad & BUTTON_R1 && i > 0x333333) goto rxTools_boot;
			}
		}
		rxModeQuickBoot();
	}
rxTools_boot:
	sprintf(str, "/rxTools/Theme/%u/TOP.bin", cfgs[CFG_THEME].val.i);
	sprintf(strl, "/rxTools/Theme/%u/TOPL.bin", cfgs[CFG_THEME].val.i);
	sprintf(strr, "/rxTools/Theme/%u/TOPR.bin", cfgs[CFG_THEME].val.i);
	if (cfgs[CFG_3D].val.i)
		DrawTopSplash(str, strl, strr);
	else
		DrawTopSplash(str, str, str);
}

int main(){
	Initialize();
	//7.X Keys stuff
	File KeyFile;
	if(FileOpen(&KeyFile, "/slot0x25KeyX.bin", 0)){
		u8 keyX[16];
		FileRead(&KeyFile, keyX, 16, 0);
		FileClose(&KeyFile);
		setup_aeskeyX(0x25, keyX);
	}else{
		if(GetSystemVersion() < 3){
			ConsoleInit();
			ConsoleSetTitle(L"%17ls", L"WARNING");
			print(L"WARNING:\n\nCannot find slot0x25KeyX.bin. If\nyour firmware version is less than\n7.X, some titles decryption will\nfail, and some EmuNANDs will not\nboot.\n\nPress Ⓐ to continue...\n");
			ConsoleShow();
			WaitForButton(BUTTON_A);
		}
	}

	//That's the Main Menu initialization, easy and cool
	MenuInit(&MainMenu);
	MenuShow();

	while (true) {
		u32 pad_state = InputWait();
		if (pad_state & (BUTTON_DOWN | BUTTON_RIGHT | BUTTON_R1)) MenuNextSelection(); //I try to support every theme style
		if (pad_state & (BUTTON_UP   | BUTTON_LEFT  | BUTTON_L1)) MenuPrevSelection();
		if(pad_state & BUTTON_A)    	MenuSelect();
		if(pad_state & BUTTON_SELECT)	ShutDown();
		if(pad_state & BUTTON_START)	returnHomeMenu();
		TryScreenShot();
		MenuShow();
	}

	FSDeInit();
	return 0;
}
