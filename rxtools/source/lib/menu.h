#ifndef MENU_H
#define MENU_H

#include "common.h"
#include "console.h"

typedef struct{
	wchar_t* Str;
	void(* Func)();
	char* gfx_splash;
}MenuEntry;

typedef struct{
	wchar_t* Name;
	MenuEntry* Option;
	int nEntryes;
	int Current;    //The current selected option
	bool Showed;    //Useful, to not refresh everything everytime
} Menu;

void MenuInit(Menu* menu);
void MenuShow();
void MenuNextSelection();
void MenuPrevSelection();
void MenuSelect();
void MenuClose();
void MenuRefresh();

extern bool bootGUI;
extern unsigned char Theme;
extern bool agb_bios;
extern bool theme_3d;
extern bool silent_boot;
extern unsigned char language;
extern Menu* MyMenu;

#endif
