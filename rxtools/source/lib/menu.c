#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "menu.h"
#include "draw.h"
#include "hid.h"

Menu* MyMenu;
Menu *MenuChain[100];
int openedMenus = 0;
char Theme = '0';
char str[100];

void MenuInit(Menu* menu){
	MyMenu = menu;
	ConsoleInit();
	MyMenu->Current = 0;
	MyMenu->Showed = 0;
	ConsoleSetTitle(MyMenu->Name);
	for(int i = 0; i < MyMenu->nEntryes; i++){
		print(MyMenu->Option[i].Str);
		print("\n");
	}
}

void MenuShow(){

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
		DrawString(BOT_SCREEN, i == MyMenu->Current ? ">" : " ", x + CHAR_WIDTH*(ConsoleGetSpacing() - 1), (i)* CHAR_WIDTH + y + CHAR_WIDTH*(ConsoleGetSpacing() + 1), ConsoleGetSpecialColor(), ConsoleGetBackgroundColor());
	}*/

	//NEW GUI:
	sprintf(str, "/rxTools/Theme/%c/%s", Theme, MyMenu->Option[MyMenu->Current].gfx_splash);
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
		MenuChain[openedMenus++] = MyMenu;
		MyMenu->Option[MyMenu->Current].Func();
		MenuInit(MenuChain[--openedMenus]);
		MenuShow();
	}
}

void MenuClose(){
	if (openedMenus > 0){
		MenuInit(MenuChain[--openedMenus]);
		MenuShow();
	}
}

void MenuRefresh(){
	ConsoleInit();
	sprintf(str, "/rxTools/Theme/%c/%s", Theme, MyMenu->Option[MyMenu->Current].gfx_splash);
	DrawBottomSplash(str);
	MyMenu->Showed = 0;
	ConsoleSetTitle(MyMenu->Name);
	for (int i = 0; i < MyMenu->nEntryes; i++){
		print("%s %s", i == MyMenu->Current ? "->" : "  ", MyMenu->Option[i].Str);
		print("\n");
	}
	int x = 0, y = 0;
	ConsoleGetXY(&x, &y);
	if (!MyMenu->Showed){
		ConsoleShow();
		MyMenu->Showed = 1;
	}
}
