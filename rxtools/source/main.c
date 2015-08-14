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
#include "MainMenu.h"
#include "crypto.h"
#include "fs.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "screenshot.h"
#include "firm.h"
#include "configuration.h"

#define FONT_ADDRESS	(void*)0x27E00000
char *cfgLang = "en.json";
const char *fontpath = "/rxTools/system/font.bin";
int fontLoaded = 1;

void LoadFont(){
	File MyFile;
	if (FileOpen(&MyFile, fontpath, 0))
	{
		FileRead(&MyFile, FONT_ADDRESS, 0x200000, 0);
		fontaddr = FONT_ADDRESS;
  		fontLoaded = 0;
	}else{
		fontLoaded = 1;
	}
}

int Initialize()
{
	char str[100];
	char strl[100];
	char strr[100];
	char tmp[256];
	wchar_t wtmp[256];
	int r;

	preloadStringsA();

	DrawString(BOT_SCREEN, strings[STR_INITIALIZING], FONT_WIDTH, SCREEN_HEIGHT-FONT_HEIGHT, WHITE, BLACK);

	if(FSInit()){
		DrawString(BOT_SCREEN, strings[STR_LOADING], BOT_SCREEN_WIDTH/2, SCREEN_HEIGHT-FONT_HEIGHT, WHITE, BLACK);
	}else{
		DrawString(BOT_SCREEN, strings[STR_FAILED], BOT_SCREEN_WIDTH/2, SCREEN_HEIGHT-FONT_HEIGHT, RED, BLACK);
		return 1;
	}

	LoadFont();
	if (fontLoaded){
		swprintf(wtmp, sizeof(wtmp)/sizeof(wtmp[0]), strings[STR_ERROR_OPENING], fontpath);
		DrawString(BOT_SCREEN, wtmp, FONT_WIDTH, SCREEN_HEIGHT-FONT_HEIGHT*2, RED, BLACK);
	}else{
		preloadStringsU();
	}

	//Console Stuff
	ConsoleSetXY(15, 20);
	ConsoleSetWH(BOT_SCREEN_WIDTH-30, SCREEN_HEIGHT-80);
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

	if (fontLoaded)
		cfgs[CFG_LANG].val.s = cfgLang;
	r = loadStrings();
	if (r) {
		sprintf(tmp, "%s/%s", langPath, cfgs[CFG_LANG].val.s);
		swprintf(wtmp, sizeof(wtmp)/sizeof(wtmp[0]), strings[STR_ERROR_OPENING], tmp);
		DrawString(BOT_SCREEN, wtmp, FONT_WIDTH, SCREEN_HEIGHT-FONT_HEIGHT*3, RED, BLACK);
	}
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
				uint32_t pad = GetInput();
				if (pad & BUTTON_R1 && i > 0x333333) goto rxTools_boot;
			}
		}
		else
		{
			ConsoleInit();
			ConsoleSetTitle(strings[STR_AUTOBOOT]);
			print(strings[STR_HOLD_BUTTON_ACTION], strings[STR_BUTTON_R], strings[STR_OPEN_MENU]);
			ConsoleShow();

			for (int i = 0; i < 0x333333 * 6; i++){
				uint32_t pad = GetInput();
				if (pad & BUTTON_R1 && i > 0x333333) goto rxTools_boot;
			}
		}
		if (cfgs[CFG_ABSYSN].val.i)
			rxMode(0);
		else
			rxMode(1);
	}
rxTools_boot:
	sprintf(str, "/rxTools/Theme/%u/TOP.bin", cfgs[CFG_THEME].val.i);
	sprintf(strl, "/rxTools/Theme/%u/TOPL.bin", cfgs[CFG_THEME].val.i);
	sprintf(strr, "/rxTools/Theme/%u/TOPR.bin", cfgs[CFG_THEME].val.i);
	if (cfgs[CFG_3D].val.i)
		DrawTopSplash(str, strl, strr);
	else
		DrawTopSplash(str, str, str);

	return 0;
}

int main(){
	if (Initialize())
		while (1);

	//7.X Keys stuff
	File KeyFile;
	const char *keyfile = "/slot0x25KeyX.bin";
	if(FileOpen(&KeyFile, keyfile, 0)){
		uint8_t keyX[16];
		FileRead(&KeyFile, keyX, 16, 0);
		FileClose(&KeyFile);
		setup_aeskeyX(0x25, keyX);
	}else{
		if (sysver < 7) {
			ConsoleInit();
			ConsoleSetTitle(strings[STR_WARNING]);
			print(strings[STR_ERROR_OPENING], keyfile);
			print(strings[STR_WARNING_KEYFILE]);
			print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
			ConsoleShow();
			WaitForButton(BUTTON_A);
		}
	}

	//That's the Main Menu initialization, easy and cool
	OpenAnimation();
	MenuInit(&MainMenu);
	MenuShow();
	while (true) {
		uint32_t pad_state = InputWait();
		if (pad_state & (BUTTON_DOWN | BUTTON_RIGHT | BUTTON_R1)) MenuNextSelection(); //I try to support every theme style
		if (pad_state & (BUTTON_UP   | BUTTON_LEFT  | BUTTON_L1)) MenuPrevSelection();
		if (pad_state & BUTTON_A)    	{ OpenAnimation(); MenuSelect(); }
		if (pad_state & BUTTON_SELECT)	{ fadeOut(); ShutDown(); }
		if (pad_state & BUTTON_START)	{ fadeOut(); returnHomeMenu(); }
		TryScreenShot();
		MenuShow();
	}

	FSDeInit();
	return 0;
}
