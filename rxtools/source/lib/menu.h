#ifndef MENU_H
#define MENU_H

#include "common.h"
#include "console.h"

typedef struct{
	char* Str;
	void(* Func)();
}MenuEntry;

typedef struct{
	char* Name;
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

#endif
