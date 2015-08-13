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
#include "configuration.h"
#include "lang.h"
#include "menu.h"
#include "draw.h"
#include "hid.h"

Menu* MyMenu;
Menu *MenuChain[100];
int openedMenus = 0;
int saved_index = 0;

void MenuInit(Menu* menu){
	MyMenu = menu;
	ConsoleInit();
	if (openedMenus == 0) MyMenu->Current = saved_index; //if we're entering the main menu, load the index
	else  MyMenu->Current = 0;
    MyMenu->Showed = 0;
	ConsoleSetTitle(MyMenu->Name);
	for(int i = 0; i < MyMenu->nEntryes; i++){
		print(L"%s\n", MyMenu->Option[i].Str);
	}
}

void MenuShow(){
	char str[100];

	//OLD TEXT MENU:
	/*int x = 0, y = 0;
	ConsoleGetXY(&x, &y);
	if (!MyMenu->Showed){
		sprintf(str, "/rxTools/Theme/%c/app.bin", Theme);
		DrawBottomSplash(str);
		ConsoleShow();
		MyMenu->Showed = 1;
	}
	for (int i = 0; i < MyMenu->nEntryes; i++){
		DrawString(BOT_SCREEN, i == MyMenu->Current ? L">" : L" ", x + CHAR_WIDTH*(ConsoleGetSpacing() - 1), (i)* CHAR_WIDTH + y + CHAR_WIDTH*(ConsoleGetSpacing() + 1), ConsoleGetSpecialColor(), ConsoleGetBackgroundColor());
	}*/

	//NEW GUI:
	sprintf(str, "/rxTools/Theme/%u/%s", cfgs[CFG_THEME].val.i, MyMenu->Option[MyMenu->Current].gfx_splash);
	DrawBottomSplash(str);
}

void MenuNextSelection(){
	if(MyMenu->Current + 1 < MyMenu->nEntryes){
		MyMenu->Current++;
	}else{
		MyMenu->Current = 0;
	}

}

void MenuPrevSelection(){
	if(MyMenu->Current > 0){
		MyMenu->Current--;
	}else{
		MyMenu->Current = MyMenu->nEntryes - 1;
	}
}

void MenuSelect(){
	if(MyMenu->Option[MyMenu->Current].Func != NULL){
		if (openedMenus == 0)saved_index = MyMenu->Current; //if leaving the main menu, save the index
		MenuChain[openedMenus++] = MyMenu;
		MyMenu->Option[MyMenu->Current].Func();
		MenuInit(MenuChain[--openedMenus]);
		MenuShow();
	}
}

void MenuClose(){
	if (openedMenus > 0){
		OpenAnimation();
	}
}

void MenuRefresh(){
	ConsoleInit();
	MyMenu->Showed = 0;
	ConsoleSetTitle(MyMenu->Name);
	for (int i = 0; i < MyMenu->nEntryes; i++){
		print(L"%ls %ls\n", i == MyMenu->Current ? strings[STR_CURSOR] : strings[STR_NO_CURSOR], MyMenu->Option[i].Str);
	}
	int x = 0, y = 0;
	ConsoleGetXY(&x, &y);
	if (!MyMenu->Showed){
		ConsoleShow();
		MyMenu->Showed = 1;
	}
}
