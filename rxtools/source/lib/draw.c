#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "fs.h"
#include "font.h"
#include "draw.h"
#include "filepack.h"

u32 current_y = 1;

u8 *tmpscreen = (u8*)0x26000000;

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

void DrawCharacter(u8 *screen, char character, u32 x, u32 y, u32 color, u32 bgcolor)
{
	u32 yy;
	u8 *screenPos, *screenStart = screen + (x * SCREEN_HEIGHT + SCREEN_HEIGHT - y - 1) * BYTES_PER_PIXEL;
	//Use cached value, yep.
	u8 foreA = color >> 24, foreR = color >> 16, foreG = color >> 8, foreB = color;
	u8 backA = bgcolor >> 24, backR = bgcolor >> 16, backG = bgcolor >> 8, backB = bgcolor;
	u32 charPos = character * FONT_SIZE;
	u8 charVal;
	for (screenPos = screenStart; screenPos < screenStart + (SCREEN_HEIGHT - FONT_SIZE) * BYTES_PER_PIXEL * FONT_SIZE; screenPos += (SCREEN_HEIGHT - FONT_SIZE) * BYTES_PER_PIXEL)
	{
		charVal = font[charPos++];
		for (yy = FONT_SIZE; yy--;)
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
	if(screen == TOP_SCREEN && TOP_SCREEN2){
		screenStart = TOP_SCREEN2 + (x * SCREEN_HEIGHT + SCREEN_HEIGHT - y - 1) * BYTES_PER_PIXEL;
		charPos = character * FONT_SIZE;
		for (screenPos = screenStart; screenPos < screenStart + (SCREEN_HEIGHT - FONT_SIZE) * BYTES_PER_PIXEL * FONT_SIZE; screenPos += (SCREEN_HEIGHT - FONT_SIZE) * BYTES_PER_PIXEL)
		{
			charVal = font[charPos++];
			for (yy = FONT_SIZE; yy--;)
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

void DrawString(u8 *screen, const char *str, u32 x, u32 y, u32 color, u32 bgcolor)
{
	for (u32 i = 0; i < strlen(str); i++){
		DrawCharacter(screen, str[i], x + i * FONT_SIZE, y, color, bgcolor);
	}
}
//[Unused]
void DrawHex(u8 *screen, u32 hex, u32 x, u32 y, u32 color, u32 bgcolor)
{
	char HexStr[4+1] = {0,}, i = sizeof(hex);
	while (i){
		HexStr[(i--)-1] = 0x30 + (hex & 0xF); hex = hex >> 4;
	}
	DrawString(screen, HexStr, x + i * FONT_SIZE, y, color, bgcolor);
}
//[Unused]
void DrawHexWithName(u8 *screen, const char *str, u32 hex, u32 x, u32 y, u32 color, u32 bgcolor)
{
	DrawString(screen, str, x, y, color, bgcolor);
	DrawHex(screen, hex, x + strlen(str) * FONT_SIZE, y, color, bgcolor);
}

void Debug(const char *format, ...)
{
	char* str;
	va_list va;

	va_start(va, format);
	vasprintf(&str, format, va);
	va_end(va);

	DrawString(TOP_SCREEN, str, 10, current_y, RGB(255, 255, 255), RGB(0, 0, 0));
	free(str);

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


//----------------Some of my shit..........
void SplashScreen(void){
	memcpy(TOP_SCREEN, GetFilePack("top_bg.bin"), SCREEN_SIZE);
	if(TOP_SCREEN2)
		memcpy(TOP_SCREEN2, GetFilePack("top_bg.bin"), SCREEN_SIZE);
}

void DrawTopSplash(char splash_file[]) {
	unsigned int n = 0, bin_size;
	File Splash;
	FileOpen(&Splash, splash_file, 0);
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

void DrawBottomSplash(char splash_file[]) {
	unsigned int n = 0, bin_size;
	File Splash;
	FileOpen(&Splash, splash_file, 0);
	//Load the spash image
	bin_size = 0;
	while ((n = FileRead(&Splash, (void*)((u32)BOT_SCREEN + bin_size), 0x100000, bin_size)) > 0) {
		bin_size += n;
	}
	u32 *fb1 = (u32*)BOT_SCREEN;
	u32 *fb2 = (u32*)BOT_SCREEN2;
	for (n = 0; n < bin_size; n += 4){
		*fb2++ = *fb1++;
	}
	FileClose(&Splash);
}