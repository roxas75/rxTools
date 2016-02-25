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

wchar_t console[CONSOLE_MAX_LINES][CONSOLE_MAX_LINE_LENGTH + 1];
wchar_t consoletitle[CONSOLE_MAX_TITLE_LENGTH+1] = L"";

int BackgroundColor = WHITE;
int TextColor = BLACK;
int BorderColor = BLUE;
int SpecialColor = RED;
int Spacing = 2;
unsigned int ConsoleX = CONSOLE_X, ConsoleY = CONSOLE_Y, ConsoleW = CONSOLE_WIDTH, ConsoleH = CONSOLE_HEIGHT;
unsigned int BorderWidth = 1;
unsigned int consoleInited = 0;

static struct {
	unsigned int col;
	unsigned int row;
} cursor = { 0, 0 };

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

int countLines()
{
	int i;

	for (i = 0; i < CONSOLE_MAX_LINES && console[i][0] != 0; i++);
	return i;
}

void ConsoleShow(){
	wchar_t str[_MAX_LFN];

	void *tmpscreen = (void*)0x27000000;
	swprintf(str, _MAX_LFN, L"/rxTools/Theme/%u/app.bin", cfgs[CFG_THEME].val.i);
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

	for (int i = 0; i < CONSOLE_MAX_LINES; i++)
		DrawString(tmpscreen, console[i],
			ConsoleX + FONT_HWIDTH * Spacing,
			i * FONT_HEIGHT + ConsoleY + 15 + FONT_HEIGHT * (Spacing - 1) + titley,
			TextColor, ConsoleGetBackgroundColor());

	memcpy(BOT_SCREEN, tmpscreen, SCREEN_SIZE);
}

void ConsoleFlush()
{
	memset(console, 0, sizeof(console));
	cursor.col = 0;
	cursor.row = 0;
}

void ConsoleAddText(wchar_t* str)
{
	wchar_t c;

	for(int i = 0; *str != 0x00; i++) {
		c = *str;

		switch (c) {
			case 0x1B:
				str++;
				if (*str != '[')
					break;

				str++;
				if (*str == 'K')
					memset(console[cursor.row] + cursor.col, 0,
						(CONSOLE_MAX_LINE_LENGTH - cursor.col) * sizeof(wchar_t));
				break;

			case '\b':
				if (cursor.col > 0)
					cursor.col--;
				break;

			case '\n':
				if (cursor.row >= CONSOLE_MAX_LINES - 1)
					break;

				cursor.row++;
			case '\r':
				cursor.col = 0;
				break;

			default:
				if (cursor.col < CONSOLE_MAX_LINE_LENGTH) {
					console[cursor.row][cursor.col] = c;
					cursor.col++;
				}
		}

		str++;
	}
}

void print(const wchar_t *format, ...)
{
	va_list va;

	va_start(va, format);
	vprint(format, va);
	va_end(va);
}

void vprint(const wchar_t *format, va_list va)
{
	wchar_t str[256];
	vswprintf(str, sizeof(str) / sizeof(str[0]), format, va);
	ConsoleAddText(str);
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
