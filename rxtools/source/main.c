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
	DrawString(TOP_SCREEN,  " INITIALIZE... ", 0, 240-8, WHITE, BLACK);
	if(FSInit()){
		DrawString(TOP_SCREEN,  " LOADING...    ", 0, 240-8, WHITE, BLACK);
	}else{
		DrawString(TOP_SCREEN,  " ERROR!        ", 0, 240-8, RED, BLACK);
	}
	LoadPack();
	//Console Stuff
	memset(TOP_SCREEN, 0x00, 0x46500);
	memset(TOP_SCREEN2, 0x00, 0x46500);
	ConsoleSetXY(15, 15);
	ConsoleSetWH(370, 160);
	ConsoleSetBorderColor(BLUE);
	ConsoleSetTextColor(WHITE);
	ConsoleSetBackgroundColor(BLACK);
	ConsoleSetSpecialColor(BLUE);
	ConsoleSetSpacing(2);
	ConsoleSetBorderWidth(3);
	//Check that the data is installed
	InstallConfigData();
	
	SplashScreen();
	for(int i = 0; i < 0x333333*6; i++){
		u32 pad = GetInput();
		if(pad & BUTTON_R1 && i > 0x333333) goto rxTools_boot;
	}
    rxModeEmu();
	rxTools_boot:
	memset(TOP_SCREEN, 0x00, 0x46500);
	memset(TOP_SCREEN2, 0x00, 0x46500);
	f_mkdir ("rxTools");
	f_mkdir ("rxTools/nand");
	//f_mkdir ("capture");
}

int main(){
	Initialize();
	DrawString(TOP_SCREEN, "SUPPORT THE ORIGINAL, NOT THE IMITATION!", 75, 240-10, GREY, BLACK);
	//7.X Keys stuff
	File KeyFile;
	if(FileOpen(&KeyFile, "/slot0x25KeyX.bin", 0)){
		u8 keyX[16];
		FileRead(&KeyFile, keyX, 16, 0);
		FileClose(&KeyFile);
		setup_aeskeyX(0x25, keyX);
		DrawString(TOP_SCREEN, " NewKeyX ", 0, 240-8, GREEN, BLACK);
	}else{
		if(GetSystemVersion() < 3){
			ConsoleInit();
			print("WARNING:\n\nCannot find slot0x25KeyX.bin.\nSome titles decryption will fail,\nand some some EmuNANDs will not boot.\n\nPress A to continue...\n");
			ConsoleShow();
			WaitForButton(BUTTON_A);
		}
		DrawString(TOP_SCREEN, " NewKeyX ", 0, 240-8, RED, WHITE);
	}
	DrawString(TOP_SCREEN, " EmuNAND ", 0, 240-16, checkEmuNAND() ? GREEN : RED, BLACK);

	//That's the Main Menu initialization, easy and cool
	MenuInit(&MainMenu);
	MenuShow();

    while (true) {
		DrawString(TOP_SCREEN,  "[SELECT] Reboot", 349-18*8, 181-24-8, RED, BLACK);
		DrawString(TOP_SCREEN,  "[START]  Shutdown", 349-18*8, 181-24, RED, BLACK);
        u32 pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) 	MenuNextSelection();
		if(pad_state & BUTTON_UP)   	MenuPrevSelection();
		if(pad_state & BUTTON_A)    	MenuSelect();
		if(pad_state & BUTTON_SELECT)	returnHomeMenu();
		if(pad_state & BUTTON_START)	ShutDown();
		TryScreenShot();
		MenuShow();
    }

    FSDeInit();
    return 0;
}
