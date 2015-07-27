#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>
#include "fs.h"
#include "font.h"
#include "draw.h"
#include "filepack.h"

u32 current_y = 1;

u8 *tmpscreen = (u8*)0x26000000;
const u8 *fontaddr = font;

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

void DrawCharacter(u8 *screen, wchar_t character, u32 x, u32 y, u32 color, u32 bgcolor)
{
	u32 yy;
	u8 *screenPos, *screenStart = screen + (x * SCREEN_HEIGHT + SCREEN_HEIGHT - y - 1) * BYTES_PER_PIXEL;
	//Use cached value, yep.
	u8 foreA = color >> 24, foreR = color >> 16, foreG = color >> 8, foreB = color;
	u8 backA = bgcolor >> 24, backR = bgcolor >> 16, backG = bgcolor >> 8, backB = bgcolor;
	u32 charPos = character * FONT_WIDTH * FONT_HEIGHT / 8;
	u16 charVal;
	for (screenPos = screenStart; screenPos < screenStart + (SCREEN_HEIGHT - FONT_HEIGHT) * BYTES_PER_PIXEL * (character<FONT_CJK_START?FONT_HWIDTH:FONT_WIDTH); screenPos += (SCREEN_HEIGHT - FONT_HEIGHT) * BYTES_PER_PIXEL)
	{
		charVal = *(u16*)(fontaddr+charPos);
		charPos+=2;
		for (yy = FONT_HEIGHT; yy--;)
		{
			if (charVal & 1)
			{
				if(foreA){
					*(screenPos++) = foreB;
					*(screenPos++) = foreG;
					*(screenPos++) = foreR;
				}
				else
				{
					screenPos += 3;
				}
			}
			else
			{
				if(backA){
					*(screenPos++) = backB;
					*(screenPos++) = backG;
					*(screenPos++) = backR;
				}
				else
				{
					screenPos += 3;
				}
			}
			charVal >>= 1;
		}
	}
	//Still i don't know if we should draw the text twice.
	if(screen == BOT_SCREEN && BOT_SCREEN2){
		screenStart = BOT_SCREEN2 + (x * SCREEN_HEIGHT + SCREEN_HEIGHT - y - 1) * BYTES_PER_PIXEL;
		u32 charPos = character * FONT_WIDTH * FONT_HEIGHT / 8;
		for (screenPos = screenStart; screenPos < screenStart + (SCREEN_HEIGHT - FONT_HEIGHT) * BYTES_PER_PIXEL * (character<FONT_CJK_START?FONT_HWIDTH:FONT_WIDTH); screenPos += (SCREEN_HEIGHT - FONT_HEIGHT) * BYTES_PER_PIXEL)
		{
			charVal = *(u16*)(fontaddr+charPos);
			charPos+=2;
			for (yy = FONT_HEIGHT; yy--;)
			{
				if (charVal & 1)
				{
					if(foreA){
						*(screenPos++) = foreB;
						*(screenPos++) = foreG;
						*(screenPos++) = foreR;
					}
					else
					{
						screenPos += 3;
					}
				}
				else
				{
					if(backA){
						*(screenPos++) = backB;
						*(screenPos++) = backG;
						*(screenPos++) = backR;
					}
					else
					{
						screenPos += 3;
					}
				}
				charVal >>= 1;
			}
		}
	}
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
	wchar_t HexStr[4+1] = {0,}, i = sizeof(hex);
	while (i){
		HexStr[(i--)-1] = '0' + (hex & 0xF);
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
	if(x >= SCREEN_WIDTH || x < 0) return;
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
		u32 *fb1 = (u32*)TOP_SCREEN;
		u32 *fb2 = (u32*)TOP_SCREEN2;
		for (n = 0; n < bin_size; n += 4){
			*fb2++ = *fb1++;
		}
		FileClose(&Splash);
	}
	else
	{
		DrawString(BOT_SCREEN, L"MISSING THEME FILES!", FONT_WIDTH, SCREEN_HEIGHT - FONT_HEIGHT, RED, BLACK);
	}
}

void DrawBottomSplash(char splash_file[]) {
	unsigned int n = 0, bin_size;
	File Splash;
	if(FileOpen(&Splash, splash_file, 0))
	{
		//Load the spash image
		bin_size = 0;
		while ((n = FileRead(&Splash, (void*)((u32)BOT_SCREEN + bin_size), 0x100000, bin_size)) > 0) {
			bin_size += n;
		}
		FileClose(&Splash);
	}
	else
	{
		DrawString(BOT_SCREEN, L"MISSING THEME FILES!", FONT_WIDTH, SCREEN_HEIGHT - FONT_HEIGHT, RED, BLACK);
	}
}
