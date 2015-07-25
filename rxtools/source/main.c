#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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

void LoadSettings(){
	char settings[]="000100";
	char str[100];
	File MyFile;
	if (FileOpen(&MyFile, "/rxTools/data/system.txt", 0))
	{
		if (FileGetSize(&MyFile) == 6)
		{
			FileRead(&MyFile, settings, 6, 0);
			bootGUI = (settings[0] == '1');
			agb_bios = (settings[2] == '1');
			theme_3d = (settings[3] == '1');
			silent_boot = (settings[4] == '1');
			
			/* Disable autostart after the first boot */
			if (first_boot && !bootGUI) bootGUI = true;
			
			/* Check if the Theme Number is valid */
			unsigned char theme_num = (settings[0] - 0x30);
			if (theme_num >= 0 && theme_num <= 9)
			{
				File Menu0;
				sprintf(str, "/rxTools/Theme/%c/menu0.bin", settings[1]);
				if (FileOpen(&Menu0, str, 0))
				{
					Theme = settings[1]; //check if the theme exists, else load theme 0 (default)
					FileClose(&Menu0);
				} else
					Theme = '0';
			} else {
				Theme = '0';
				FileWrite(&MyFile, &Theme, 1, 1);
			}

			/* Check if the Language Number is valid */
			if(settings[5] - 0x30 >= 0 && settings[5] - 0x30 <= N_LANG)
				language = settings[5] - 0x30;
			else
				language = 0;

			FileClose(&MyFile);
			return;
		} else {
			FileClose(&MyFile);
		}
	}
	
	/* Disable autostart after the first boot */
	bootGUI = first_boot;
	Theme = '0';
	agb_bios = false;
	
	/* Create system.txt */
	if (FileOpen(&MyFile, "/rxTools/data/system.txt", 1))
	{
		FileWrite(&MyFile, settings, 6, 0);
		FileClose(&MyFile);
	}
}

void Initialize(){
	char str[100];
	char strl[100];
	char strr[100];
	DrawString(BOT_SCREEN,  " INITIALIZE... ", 0, SCREEN_HEIGHT-FONT_SIZE, WHITE, BLACK);
	if(FSInit()){
		DrawString(BOT_SCREEN,  " LOADING...    ", 0, SCREEN_HEIGHT-FONT_SIZE, WHITE, BLACK);
	}else{
		DrawString(BOT_SCREEN,  " ERROR!        ", 0, SCREEN_HEIGHT-FONT_SIZE, RED, BLACK);
	}
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
	LoadSettings();

	sprintf(str, "/rxTools/Theme/%c/TOP.bin", Theme);
	sprintf(strl, "/rxTools/Theme/%c/TOPL.bin", Theme);
	sprintf(strr, "/rxTools/Theme/%c/TOPR.bin", Theme);
	if(theme_3d)
		DrawTopSplash(str, strl, strr);
	else
		DrawTopSplash(str, str, str);

	if (!bootGUI)
	{
		if(silent_boot)
		{
			sprintf(str, "/rxTools/Theme/%c/boot.bin", Theme);
			DrawBottomSplash(str);

			for (int i = 0; i < 0x333333 * 2; i++){
				u32 pad = GetInput();
				if (pad & BUTTON_R1 && i > 0x333333) goto rxTools_boot;
			}
		}
		else
		{
			ConsoleInit();
			ConsoleSetTitle(STR_AUTOBOOT[language]);
			print(STR_HOLD_R[language]);
			ConsoleShow();

			for (int i = 0; i < 0x333333 * 6; i++){
				u32 pad = GetInput();
				if (pad & BUTTON_R1 && i > 0x333333) goto rxTools_boot;
			}
		}
				

		rxModeQuickBoot();
	}
rxTools_boot:

	sprintf(str, "/rxTools/Theme/%c/TOP.bin", Theme);
	sprintf(strl, "/rxTools/Theme/%c/TOPL.bin", Theme);
	sprintf(strr, "/rxTools/Theme/%c/TOPR.bin", Theme);
	if(theme_3d)
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
			ConsoleSetTitle("          WARNING");
			print("WARNING:\n\nCannot find slot0x25KeyX.bin. If\nyour firmware version is less than\n7.X, some titles decryption will\nfail, and some EmuNANDs will not\nboot.\n\nPress A to continue...\n");
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
