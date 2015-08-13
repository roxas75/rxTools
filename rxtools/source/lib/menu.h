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

#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <wchar.h>
#include "console.h"

typedef struct{
	wchar_t Str[CONSOLE_MAX_LINE_LENGTH+1];
	void(* Func)();
	char* gfx_splash;
}MenuEntry;

typedef struct{
	wchar_t Name[CONSOLE_MAX_LINE_LENGTH+1];
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
