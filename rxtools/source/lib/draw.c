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

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>
#include "fs.h"
#include "font.h"
#include "draw.h"
#include "lang.h"

u32 current_y = 1;

u8 *tmpscreen = (u8*)0x26000000;
const u16 (* fontaddr)[FONT_WIDTH] = (void *)font;

void ClearScreen(u8 *screen, u32 color)
{
	u32 i = SCREEN_SIZE/sizeof(u32);  //Surely this is an interger.
	u32* tmpscr = (u32*)screen; //To avoid using array index which would decrease speed.
	color &= COLOR_MASK; //Ignore aplha
	//Prepared 3 u32, that includes 4 24-bits color, cached. 4x(BGR)
	u32 color0 = (color) | (color << 24),
		color1 = (color << 16) | (color >> 8),
		color2 = (color >> 16) | (color << 8);
	while (i--) {
		*(tmpscr++) = color0;
		*(tmpscr++) = color1;
		*(tmpscr++) = color2;
	}
}

void DrawClearScreenAll(void) {
	ClearScreen(TOP_SCREEN, RGB(0, 0, 0));
	ClearScreen(TOP_SCREEN2, RGB(0, 0, 0));
	ClearScreen(BOT_SCREEN, RGB(0, 0, 0));
	ClearScreen(BOT_SCREEN2, RGB(0, 0, 0));
	current_y = 0;
}

static void DrawCharacterOn1frame(void *screen, wchar_t character, u32 x, u32 y, u32 color, u32 bgcolor)
{
	struct {
		u8 a;
		u8 r;
		u8 g;
		u8 b;
	} fore, back;
	u8 (* pScreen)[SCREEN_HEIGHT][BYTES_PER_PIXEL];
	u32 fontX, fontY;
	u16 charVal;

	if (BOT_SCREEN_WIDTH < x + FONT_WIDTH || y < FONT_HEIGHT)
		return;

	fore.a = color >> 24;
	fore.r = color >> 16;
	fore.g = color >> 8;
	fore.b = color;

	back.a = bgcolor >> 24;
	back.r = bgcolor >> 16;
	back.g = bgcolor >> 8;
	back.b = color;

	pScreen = screen;

	for (fontX = 0; fontX < FONT_WIDTH; fontX++) {
		charVal = fontaddr[character][fontX];
		for (fontY = 0; fontY < FONT_HEIGHT; fontY++) {
			if (charVal & 1) {
				if (fore.a) {
					pScreen[x][SCREEN_HEIGHT - (y - fontY)][0] = fore.b;
					pScreen[x][SCREEN_HEIGHT - (y - fontY)][1] = fore.g;
					pScreen[x][SCREEN_HEIGHT - (y - fontY)][2] = fore.r;
				}
			} else {
				if (back.a) {
					pScreen[x][SCREEN_HEIGHT - (y - fontY)][0] = back.b;
					pScreen[x][SCREEN_HEIGHT - (y - fontY)][1] = back.g;
					pScreen[x][SCREEN_HEIGHT - (y - fontY)][2] = back.r;
				}
			}

			charVal >>= 1;
		}

		x++;
	}
}

void DrawCharacter(u8 *screen, wchar_t character, u32 x, u32 y, u32 color, u32 bgcolor)
{
	DrawCharacterOn1frame(screen, character, x, y, color, bgcolor);

	if (screen == BOT_SCREEN && BOT_SCREEN2)
		DrawCharacterOn1frame(BOT_SCREEN2, character, x, y, color, bgcolor);
}

void DrawString(u8 *screen, const wchar_t *str, u32 x, u32 y, u32 color, u32 bgcolor)
{
	unsigned int dx = 0;
	for (u32 i = 0; i < wcslen(str); i++){
		DrawCharacter(screen, str[i], x + dx, y, color, bgcolor);
		dx+=str[i]<FONT_CJK_START?FONT_HWIDTH:FONT_WIDTH;
	}
}
//[Unused]
void DrawHex(u8 *screen, u32 hex, u32 x, u32 y, u32 color, u32 bgcolor)
{
	u32 i = sizeof(hex)*2;
	wchar_t HexStr[sizeof(hex)*2+1] = {0,};
	while (i){
		HexStr[--i] = hex & 0x0F;
		HexStr[i] += HexStr[i] > 9 ? '7' : '0';
		hex >>= 4;
	}
	DrawString(screen, HexStr, x, y, color, bgcolor);
}
//[Unused]
void DrawHexWithName(u8 *screen, const wchar_t *str, u32 hex, u32 x, u32 y, u32 color, u32 bgcolor)
{
	DrawString(screen, str, x, y, color, bgcolor);
	DrawHex(screen, hex, x + wcslen(str) * FONT_HWIDTH, y, color, bgcolor);
}

void Debug(const char *format, ...)
{
	char *str;
	va_list va;

	va_start(va, format);
	vasprintf(&str, format, va);
	va_end(va);
	wchar_t wstr[strlen(str)+1];
	mbstowcs(wstr, str, strlen(str)+1);
	free(str);
	DrawString(TOP_SCREEN, wstr, 10, current_y, RGB(255, 255, 255), RGB(0, 0, 0));

	current_y += 10;
}
//No need to enter and exit again and again, isn't it
inline void writeByte(u8 *address, u8 value) {
	*(address) = value;
}

void DrawPixel(u8 *screen, u32 x, u32 y, u32 color){
	if(x >= BOT_SCREEN_WIDTH || x < 0) return;
	if(y >= SCREEN_HEIGHT || y < 0) return;
	if(color & ALPHA_MASK){
		u8 *address  = screen + (SCREEN_HEIGHT * (x + 1) - y) * BYTES_PER_PIXEL;
		writeByte(address, color);
		writeByte(address+1, color >> 8);
		writeByte(address+2, color >> 16);
	}
	if(screen == TOP_SCREEN && TOP_SCREEN2){
		if(color & ALPHA_MASK){
			u8 *address = TOP_SCREEN2 + (SCREEN_HEIGHT * (x + 1) - y) * BYTES_PER_PIXEL;
			writeByte(address, color);
			writeByte(address+1, color >> 8);
			writeByte(address+2, color >> 16);
		}
	}
}

u32 GetPixel(u8 *screen, u32 x, u32 y){
	return *(u32*)(screen + (SCREEN_HEIGHT * (x + 1) - y) * BYTES_PER_PIXEL) & COLOR_MASK;
}


//----------------New Splash Screen Stuff------------------

void DrawTopSplash(char splash_file[], char splash_fileL[], char splash_fileR[]) {
	unsigned int n = 0, bin_size;
	File Splash, SplashL, SplashR;
	if (FileOpen(&SplashL, splash_fileL, 0)&&FileOpen(&SplashR, splash_fileR, 0))
	{
		//Load the spash image
		bin_size = 0;
		while ((n = FileRead(&SplashL, (void*)((u32)TOP_SCREEN + bin_size), 0x100000, bin_size)) > 0) {
			bin_size += n;
		}
		FileClose(&SplashL);
		bin_size = 0;
		while ((n = FileRead(&SplashR, (void*)((u32)TOP_SCREEN2 + bin_size), 0x100000, bin_size)) > 0) {
			bin_size += n;
		}
		FileClose(&SplashR);
	}
	else if (FileOpen(&Splash, splash_file, 0))
	{
		//Load the spash image
		bin_size = 0;
		while ((n = FileRead(&Splash, (void*)((u32)TOP_SCREEN + bin_size), 0x100000, bin_size)) > 0) {
			bin_size += n;
		}
		FileClose(&Splash);
		memcpy(TOP_SCREEN2, TOP_SCREEN, bin_size);
	}
	else
	{
		wchar_t tmp[256];
		swprintf(tmp, sizeof(tmp)/sizeof(tmp[0]), strings[STR_ERROR_OPENING], splash_file);
		DrawString(BOT_SCREEN, tmp, FONT_WIDTH, SCREEN_HEIGHT - FONT_HEIGHT, RED, BLACK);
	}
}

void DrawBottomSplash(char splash_file[]) {
	DrawSplash(BOT_SCREEN, splash_file);
}

void DrawSplash(u8 *screen, char splash_file[]) {
	unsigned int n = 0, bin_size;
	File Splash;
	if(FileOpen(&Splash, splash_file, 0))
	{
		//Load the spash image
		bin_size = 0;
		while ((n = FileRead(&Splash, (void*)((u32)screen + bin_size), 0x100000, bin_size)) > 0) {
			bin_size += n;
		}
		FileClose(&Splash);
	}
	else
	{
		wchar_t tmp[256];
		swprintf(tmp, sizeof(tmp)/sizeof(tmp[0]), strings[STR_ERROR_OPENING], splash_file);
		DrawString(BOT_SCREEN, tmp, FONT_WIDTH, SCREEN_HEIGHT - FONT_HEIGHT, RED, BLACK);
	}
}

void DrawFadeScreen(u8 *screen, u16 Width, u16 Height, u32 f)
{
	int i; for (i = 0; i<Width*Height / 2; i++)
	{
		*screen = (*screen*f) >> 8; screen++;
		*screen = (*screen*f) >> 8; screen++;
		*screen = (*screen*f) >> 8; screen++;
		*screen = (*screen*f) >> 8; screen++;
		*screen = (*screen*f) >> 8; screen++;
		*screen = (*screen*f) >> 8; screen++;
	}
}

void fadeOut(){
	for (int x = 255; x >= 0; x = x - 15){
		DrawFadeScreen(BOT_SCREEN, BOT_SCREEN_WIDTH, SCREEN_HEIGHT, x);
		DrawFadeScreen(TOP_SCREEN, TOP_SCREEN_WIDTH, SCREEN_HEIGHT, x);
		DrawFadeScreen(TOP_SCREEN2, TOP_SCREEN_WIDTH, SCREEN_HEIGHT, x);
	}
}