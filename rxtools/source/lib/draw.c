#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "font.h"
#include "draw.h"
#include "filepack.h"

u32 current_y = 1;

u8 *tmpscreen = (u8*)0x26000000;

void ClearScreen(u8 *screen, u32 color)
{
	u32 i = SCREEN_SIZE/sizeof(u32);  //Surely this is an interger.
	u32* tmpscr = (u32*)screen; //To avoid using array index which would decrease speed.
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

void ClearScreen2(u8 *screen, u32 color)
{
	u32 i = SCREEN_SIZE2/sizeof(u32);  //Surely this is an interger.
	u32* tmpscr = (u32*)screen; //To avoid using array index which would decrease speed.
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

void DrawCharacter(u8 *screen, char character, u32 x, u32 y, u32 color, u32 bgcolor)
{
	u32 yy, xx;
	u32 xDisplacement = x * SCREEN_HEIGHT;
	u32 yDisplacement = SCREEN_HEIGHT - y - 1;
	u8 *screenPos = screen + (xDisplacement + yDisplacement) * BYTES_PER_PIXEL;
	//Use cached value, yep.
	u8 foreR = color >> 16, foreG = color >> 8, foreB = color;
	u8 backR = bgcolor >> 16, backG = bgcolor >> 8, backB = bgcolor;
	for (yy = 0; yy < FONT_SIZE; yy++)
	{
		u8 charPos = font[character * FONT_SIZE + yy];
		for (xx = 0; xx < FONT_SIZE; xx++)
		{
			if ((charPos << xx) & 0x80)
			{
				if(color & ALPHA_MASK){
					*(screenPos++) = foreB;
					*(screenPos++) = foreG;
					*(screenPos++) = foreR;
				}
			}
			else
			{
				if(bgcolor & ALPHA_MASK){
					*(screenPos++) = backB;
					*(screenPos++) = backG;
					*(screenPos++) = backR;
				}
			}
			screenPos += BYTES_PER_PIXEL * SCREEN_HEIGHT - 3;
		}
		screenPos -= BYTES_PER_PIXEL * (SCREEN_HEIGHT * FONT_SIZE + 1);
	}
	//Still i don't know if we should draw the text twice.
	if(screen == TOP_SCREEN && TOP_SCREEN2){
		screen = TOP_SCREEN2;
		u32 xDisplacement = x * SCREEN_HEIGHT;
		u32 yDisplacement = SCREEN_HEIGHT - y - 1;
		u8 *screenPos = screen + (xDisplacement + yDisplacement) * BYTES_PER_PIXEL;
		for (yy = 0; yy < FONT_SIZE; yy++)
		{
			u8 charPos = font[character * FONT_SIZE + yy];
			for (xx = 0; xx < FONT_SIZE; xx++)
			{
				if ((charPos << xx) & 0x80)
				{
					if(color & ALPHA_MASK){
						*(screenPos++) = foreB;
						*(screenPos++) = foreG;
						*(screenPos++) = foreR;
					}
				}
				else
				{
					if(bgcolor & ALPHA_MASK){
						*(screenPos++) = backB;
						*(screenPos++) = backG;
						*(screenPos++) = backR;
					}
				}
			screenPos += BYTES_PER_PIXEL * SCREEN_HEIGHT - 3;
			}
		screenPos -= BYTES_PER_PIXEL * (SCREEN_HEIGHT * FONT_SIZE + 1);
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
void SplashScreen2(void){
	ClearScreen2(BOT_SCREEN, BLACK);
	memcpy(BOT_SCREEN, GetFilePack("top_bg2.bin"), SCREEN_SIZE2);
}