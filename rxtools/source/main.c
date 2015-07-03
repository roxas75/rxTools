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
	DrawString(TOP_SCREEN,  " INITIALIZE... ", 0, SCREEN_HEIGHT-FONT_SIZE, WHITE, BLACK);
	if(FSInit()){
		DrawString(TOP_SCREEN,  " LOADING...    ", 0, SCREEN_HEIGHT-FONT_SIZE, WHITE, BLACK);
	}else{
		DrawString(TOP_SCREEN,  " ERROR!        ", 0, SCREEN_HEIGHT-FONT_SIZE, RED, BLACK);
	}
	LoadPack();
	//Console Stuff
	memset(TOP_SCREEN, 0x00, SCREEN_SIZE);
	memset(TOP_SCREEN2, 0x00, SCREEN_SIZE);
	memset(BOT_SCREEN, 0x00, SCREEN_SIZE2);
	SplashScreen2();
	ConsoleSetXY(15, 15);
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
	
	SplashScreen();
	DrawString(TOP_SCREEN, " Hold [R] button to enter rxTools Menu... ", 0, SCREEN_HEIGHT-12, WHITE, BLACK);
	for(int i = 0; i < 0x111111*6; i++){
		u32 pad = GetInput();
		if(pad & BUTTON_R1 && i > 0x111111) goto rxTools_boot;
	}
	SplashScreen();
	DrawString(TOP_SCREEN,  " Booting rxMode now... ", 0, SCREEN_HEIGHT-12, WHITE, BLACK);
	rxModeQuickBoot();
	rxTools_boot:
	memset(TOP_SCREEN, 0x00, SCREEN_SIZE);
	memset(TOP_SCREEN2, 0x00, SCREEN_SIZE);
	memset(BOT_SCREEN, 0x00, SCREEN_SIZE2);
}

int main(){
	Initialize();
	SplashScreen2();
	DrawString(TOP_SCREEN, "SUPPORT THE ORIGINAL, NOT THE IMITATION!", 75, SCREEN_HEIGHT-10, GREY, BLACK);
	//7.X Keys stuff
	File KeyFile;
	if(FileOpen(&KeyFile, "/slot0x25KeyX.bin", 0)){
		u8 keyX[16];
		FileRead(&KeyFile, keyX, 16, 0);
		FileClose(&KeyFile);
		setup_aeskeyX(0x25, keyX);
		DrawString(TOP_SCREEN, " NewKeyX ", 0, SCREEN_HEIGHT-FONT_SIZE, GREEN, BLACK);
	}else{
		//if(GetSystemVersion() < 3){
			//ConsoleInit();
			//print("WARNING:\n\nCannot find slot0x25KeyX.bin.\nSome titles decryption will fail,\nand some EmuNANDs will not boot.\n\nPress A to continue...\n");
			//ConsoleShow();
			//WaitForButton(BUTTON_A);
		//}
		DrawString(TOP_SCREEN, " Can't find slot0x25KeyX.bin! ", 0, SCREEN_HEIGHT-56, RED, BLACK);
		DrawString(TOP_SCREEN, " Some features/NAND requiring 7x keys won't work! ", 0, SCREEN_HEIGHT-46, RED, BLACK);
		DrawString(TOP_SCREEN, " NewKeyX ", 0, SCREEN_HEIGHT-FONT_SIZE, RED, BLACK);
	}
	DrawString(TOP_SCREEN, " EmuNAND ", 0, SCREEN_HEIGHT-FONT_SIZE*2, checkEmuNAND() ? GREEN : RED, BLACK);

	//That's the Main Menu initialization, easy and cool
	MenuInit(&MainMenu);
	MenuShow();

	while (true) {
		DrawString(TOP_SCREEN,  "[SELECT] Reboot", SCREEN_WIDTH-51-18*FONT_SIZE, SCREEN_HEIGHT-59-24-FONT_SIZE, RED, BLACK);
		DrawString(TOP_SCREEN,  "[START]  Shutdown", SCREEN_WIDTH-51-18*FONT_SIZE, SCREEN_HEIGHT-59-24, RED, BLACK);
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
