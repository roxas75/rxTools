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

bool bootGUI;

void LoadSettings(){
	char settings[]="00"; 
	char str[100];
	File MyFile;
	if (FileOpen(&MyFile, "/rxTools/data/system.txt", 0)){
		FileRead(&MyFile, settings, 2, 0);
		if (settings[0] == '1')bootGUI = true;
		sprintf(str, "/rxTools/Theme/%c/menu0.bin", settings[1]);
		if (FileOpen(&MyFile, str, 0)) Theme = settings[1]; //check if the theme exists, else load theme 0 (default)
		else Theme = '0';
		FileClose(&MyFile);
	}
	else
	{
		FileOpen(&MyFile, "/rxTools/data/system.txt", 1);
		FileWrite(&MyFile, "00" , 2, 0);
		Theme = '0';
		FileClose(&MyFile);
	}
}

void Initialize(){
	char str[100];
	

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
	ConsoleSetBackgroundColor(BLACK);
	ConsoleSetSpecialColor(BLUE);
	ConsoleSetSpacing(2);
	ConsoleSetBorderWidth(3);
	//Check that the data is installed
	f_mkdir ("rxTools");
	f_mkdir ("rxTools/nand");
	InstallConfigData();
	LoadSettings();

	if (!bootGUI)
	{
		for (int i = 0; i < 0x333333 * 6; i++){
			u32 pad = GetInput();
			if (pad & BUTTON_R1 && i > 0x333333) goto rxTools_boot;
		}
		rxModeQuickBoot();
	}
rxTools_boot:

	sprintf(str, "/rxTools/Theme/%c/TOP.bin", Theme);
	DrawTopSplash(str);
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
			print("WARNING:\n\nCannot find slot0x25KeyX.bin.\nSome titles decryption will fail,\nand some EmuNANDs will not boot.\n\nPress A to continue...\n");
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
		if (MyMenu->Current == 0) //If we're in the boot screen
		{
			if (pad_state & BUTTON_Y) rxModeEmu();      //Boot emunand
			else if (pad_state & BUTTON_X) rxModeSys(); //Boot sysnand
		}
		if(pad_state & BUTTON_SELECT)	returnHomeMenu();
		if(pad_state & BUTTON_START)	ShutDown();
		TryScreenShot();
		MenuShow();
	}

	FSDeInit();
	return 0;
}
