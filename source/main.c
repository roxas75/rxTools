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

void Initialize()
{
	LoadPack();
	SplashScreen();
	ConsoleSetXY(20, 20);
	ConsoleSetWH(300, 150);
	ConsoleSetBorderColor(GREY);
	ConsoleSetTextColor(WHITE);
	ConsoleSetBackgroundColor(BLACK);
	ConsoleSetBorderWidth(3);
}

int main(){
	DrawString(TOP_SCREEN,  " INITIALIZE... ", 0, 240-8, WHITE, BLACK);
	if(InitFS()){
		DrawString(TOP_SCREEN,  " LOADING...    ", 0, 240-8, WHITE, BLACK);
	}else{
		DrawString(TOP_SCREEN,  " ERROR!        ", 0, 240-8, RED, BLACK);
	}
	Initialize();
	//7.X Keys stuff
	File KeyFile;
	if(FileOpen(&KeyFile, "/slot0x25KeyX.bin", 0)){
		u8 keyX[16];
		FileRead(&KeyFile, keyX, 16, 0);
		FileClose(&KeyFile);
		setup_aeskeyX(0x25, keyX);
		DrawString(TOP_SCREEN, " NewKeyX ", 0, 240-8, GREEN, BLACK);
	}else{
		ConsoleInit();
		ConsoleAddText("WARNING:\n \nCannot find slot0x25KeyX.bin.\nIf your console firmware is\nless than 7.0, some titles\ndecryption will fail.\n \nPress A to continue...");
		ConsoleShow();
		WaitForButton(BUTTON_A);
		DrawString(TOP_SCREEN, " NewKeyX ", 0, 240-8, RED, BLACK);
	}
	DrawString(TOP_SCREEN, " EmuNAND ", 0, 240-16, checkEmuNAND() ? GREEN : RED, BLACK);
	
	//That's the Main Menu initialization, easy and cool
	MenuInit(&MainMenu);
	MenuShow();
	
    while (true) {
        u32 pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) MenuNextSelection();
		if(pad_state & BUTTON_UP)   MenuPrevSelection();
		if(pad_state & BUTTON_A)    MenuSelect();
		TryScreenShot();
		MenuShow();
    }
	
    DeinitFS();
    return 0;
}
