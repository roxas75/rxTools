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

//This is actually some basic code, i do not expect anyone to like it, i'm the first to say that's messed up.
//The fact there is that rxtools doesn't really need any specific console code for now, so i've written something simple.
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>
#include "configuration.h"
#include "console.h"
#include "draw.h"
#include "menu.h"

wchar_t console[CONSOLE_SIZE];
wchar_t consoletitle[CONSOLE_MAX_TITLE_LENGTH+1] = L"";

int BackgroundColor = WHITE;
int TextColor = BLACK;
int BorderColor = BLUE;
int SpecialColor = RED;
int Spacing = 2;
unsigned int ConsoleX = CONSOLE_X, ConsoleY = CONSOLE_Y, ConsoleW = CONSOLE_WIDTH, ConsoleH = CONSOLE_HEIGHT;
unsigned int BorderWidth = 1;
unsigned int cursor = 0;
unsigned int linecursor = 0;
unsigned int consoleInited = 0;

void ConsoleInit(){
	consoleInited = 1;
	ConsoleFlush();
}

void ConsoleSetXY(int x, int y){
	ConsoleX = x;
	ConsoleY = y;
}

void ConsoleGetXY(int *x, int *y){
	*x = ConsoleX;
	*y = ConsoleY;
}

void ConsoleSetWH(int width, int height){
	ConsoleW = width;
	ConsoleH = height;
}

void ConsoleSetBorderWidth(int width){
	BorderWidth = width;
}

int ConsoleGetBorderWidth(int width){
	return BorderWidth;
}

void ConsoleSetTitle(const wchar_t* format, ...){
	va_list va;
	va_start(va, format);
	vswprintf(consoletitle, sizeof(consoletitle)/sizeof(consoletitle[0]), format, va);
	va_end(va);
}

int countLines(){
	int cont = 0;
	for(int i = 0; i < CONSOLE_SIZE; i++){
		if(console[i] == L'\n'); cont++;
	}
	return cont;
}

int findCursorLine(){
	int cont = 0;
	for(int i = 0; i < cursor; i++){
		if(console[i] == L'\n'); cont++;
	}
	return cont;
}
void ConsoleShow(){
	char str[100];

	void *tmpscreen = (void*)0x27000000;
	sprintf(str, "/rxTools/Theme/%u/app.bin", cfgs[CFG_THEME].val.i);
	DrawSplash(tmpscreen, str);
	if(!consoleInited) return;
	int titley = 2*FONT_HEIGHT;

	//for(int y = ConsoleY; y < ConsoleH + ConsoleY + BorderWidth; y++){
	//	for(int x = ConsoleX; x < ConsoleW + ConsoleX + BorderWidth; x++){
	//		if(//(x >= ConsoleX && x <= ConsoleX + BorderWidth) ||
	//		   //(x >= ConsoleW + ConsoleX - 1 && x <= ConsoleW + ConsoleX - 1 + BorderWidth) ||
	//		   (y >= ConsoleY && y <= ConsoleY + BorderWidth) ||
	//		   (y >= ConsoleH + ConsoleY - 1 && y <= ConsoleH + ConsoleY - 1 + BorderWidth) ||
	//		   (y >= ConsoleY + titley - BorderWidth && y <= ConsoleY + titley)){
	//			DrawPixel(x, y, BorderColor, (int)tmpscreen);
	//		}else{
	//			DrawPixel(x, y, BackgroundColor, (int)tmpscreen);
	//		}
	//	}
	//}
	int titlespace = 2*FONT_HEIGHT-2*BorderWidth;
	DrawString(tmpscreen, consoletitle, ConsoleX + BorderWidth + 2 * FONT_HWIDTH, ConsoleY + (titlespace - FONT_HEIGHT) / 2 + BorderWidth, TextColor, ConsoleGetBackgroundColor());

	wchar_t tmp[256], *point;
        if(findCursorLine() < CONSOLE_MAX_LINES) point = &console[0];
	else{
		int cont = 0;
		int tmp1;
		for(tmp1 = cursor; tmp1 >= 0 && cont <= CONSOLE_MAX_LINES + 1; tmp1--){
			if(console[tmp1] == L'\n') cont++;
		}
		while(console[tmp1] != 0x00 && console[tmp1] != L'\n') tmp1--;
		point = &console[tmp1+1];
	}
	int lines = 0;
	for(int i = 0; i < CONSOLE_SIZE; i++){
		int linelen = 0;
		memset(tmp, 0, sizeof(tmp));
		while(1){
			if(*point == 0x00)  break;
			if(*point == L'\n'){ point++; break; }
			tmp[linelen++] = *point++;
		}
		DrawString(tmpscreen, tmp, ConsoleX + FONT_HWIDTH*Spacing, lines++ * FONT_HEIGHT + ConsoleY + 15 + FONT_HEIGHT*(Spacing - 1) + titley, TextColor, ConsoleGetBackgroundColor());
		if(!*point) break;
		if(lines == CONSOLE_MAX_LINES) break;
	}
	memcpy(BOT_SCREEN, tmpscreen, SCREEN_SIZE);
	if (BOT_SCREEN2) memcpy(BOT_SCREEN2, tmpscreen, SCREEN_SIZE);
}

void ConsoleFlush(){
	memset(console, 0, CONSOLE_SIZE*sizeof(console[0]));
	cursor = 0;
	linecursor = 0;
}

void ConsoleAddText(wchar_t* str){
	for(int i = 0; *str != 0x00; i++){
		if(!(*str == L'\\' && *(str+1) == L'n')){	//we just handle the '\n' case, who cares of the rest
			console[cursor++] = *str++;
			linecursor++;
		}else{
			linecursor = 0;
			console[cursor++] = L'\n';
			str += 2;
		}
	}
}

void print(const wchar_t *format, ...){
	wchar_t str[256];
	va_list va;

	va_start(va, format);
	vswprintf(str, sizeof(str)/sizeof(str[0]), format, va);
	va_end(va);
	ConsoleAddText(str);
}

void ConsoleNextLine(){
	while(console[cursor] != L'\n'){
		if(console[cursor] == 0x00){
			cursor-=2;
			break;
		}
		cursor++;
	}
	cursor++;
}

void ConsolePrevLine(){
	if(console[cursor-1] == L'\n') cursor-=2;
	while(console[cursor] != L'\n'){
		if(cursor == 0){
			break;
		}
		cursor--;
	}
	if(cursor) cursor++;
	if(cursor < 0) cursor = 0;
}

void ConsoleNext(){
	if(console[cursor + 1] != 0x00) cursor++;
}

void ConsolePrev(){
	if(cursor - 1 >= 0 ) cursor--;
}

void ConsoleSetBackgroundColor(int color){
	BackgroundColor = color;
}

int ConsoleGetBackgroundColor(){
	return BackgroundColor;
}

void ConsoleSetBorderColor(int color){
	BorderColor = color;
}

int ConsoleGetBorderColor(){
	return BorderColor;
}

void ConsoleSetTextColor(int color){
	TextColor = color;
}

int ConsoleGetTextColor(){
	return TextColor;
}

void ConsoleSetSpecialColor(int color){
	SpecialColor = color;
}

int ConsoleGetSpecialColor(){
	return SpecialColor;
}

void ConsoleSetSpacing(int space){
	Spacing = space;
}

int ConsoleGetSpacing(){
	return Spacing;
}
