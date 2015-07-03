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

void Initialize(){
	char str[100];
	DrawClearScreenAll();

	DrawString(BOT_SCREEN,  " INITIALIZE... ", 0, SCREEN_HEIGHT-FONT_SIZE, WHITE, BLACK);
	if(FSInit()){
		DrawString(BOT_SCREEN,  " LOADING...    ", 0, SCREEN_HEIGHT-FONT_SIZE, WHITE, BLACK);
	}else{
		DrawString(BOT_SCREEN,  " ERROR!        ", 0, SCREEN_HEIGHT-FONT_SIZE, RED, BLACK);
	}
	LoadPack();
	//Console Stuff

	ConsoleSetXY(15, 1);
	ConsoleSetWH(SCREEN_WIDTH-30, SCREEN_HEIGHT-80);
	ConsoleSetBorderColor(BLUE);
	ConsoleSetTextColor(WHITE);
	ConsoleSetBackgroundColor(BLACK);
	ConsoleSetSpecialColor(BLUE);
	ConsoleSetSpacing(2);
	ConsoleSetBorderWidth(3);
	//Check that the data is installed
	f_mkdir ("rxTools");
	f_mkdir ("rxTools/nand");
	InstallConfigData();
	
	for(int i = 0; i < 0x333333*6; i++){
		u32 pad = GetInput();
		if(pad & BUTTON_R1 && i > 0x333333) goto rxTools_boot;
	}
	rxModeQuickBoot();
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
		DrawString(BOT_SCREEN, " NewKeyX ", 0, SCREEN_HEIGHT-FONT_SIZE, GREEN, BLACK);
	}else{
		if(GetSystemVersion() < 3){
			ConsoleInit();
			print("WARNING:\n\nCannot find slot0x25KeyX.bin.\nSome titles decryption will fail,\nand some EmuNANDs will not boot.\n\nPress A to continue...\n");
			ConsoleShow();
			WaitForButton(BUTTON_A);
		}
		DrawString(BOT_SCREEN, " NewKeyX ", 0, SCREEN_HEIGHT-FONT_SIZE, RED, BLACK);
	}
	DrawString(BOT_SCREEN, " EmuNAND ", 0, SCREEN_HEIGHT-FONT_SIZE*2, checkEmuNAND() ? GREEN : RED, BLACK);

	//That's the Main Menu initialization, easy and cool
	MenuInit(&MainMenu);
	MenuShow();

	while (true) {
		u32 pad_state = InputWait();
		if (pad_state & BUTTON_DOWN || pad_state & BUTTON_RIGHT) 	MenuNextSelection();
		if (pad_state & BUTTON_UP || pad_state & BUTTON_LEFT)   	MenuPrevSelection();
		if(pad_state & BUTTON_A)    	MenuSelect();
		if(pad_state & BUTTON_SELECT)	returnHomeMenu();
		if(pad_state & BUTTON_START)	ShutDown();
		TryScreenShot();
		MenuShow();
	}

	FSDeInit();
	return 0;
}
