#pragma once

#include "common.h"
//Screen Macros
#define BYTES_PER_PIXEL	3  //Our color buffer accepts 24-bits color.
#define SCREEN_WIDTH	400
#define SCREEN_HEIGHT	240
#define SCREEN_SIZE	(BYTES_PER_PIXEL*SCREEN_WIDTH*SCREEN_HEIGHT)
#define FONT_SIZE	8
#define TOP_SCREEN	(u8*)(*(u32*)((u32)0x080FFFC0 + 4 * (*(u32*)0x080FFFD8 & 1)))
#define TOP_SCREEN2	(u8*)(*(u32*)0x080FFFC4)
#define BOT_SCREEN	(u8*)(*(u32*)0x080FFFD4)

//Colors Macros
#define ARGB(a,r,g,b)	(a<<24|r<<16|g<<8|b) //console asks for B,G,R in bytes
#define RGB(r,g,b)	ARGB(255,r,g,b) //opaque color
#define COLOR_MASK	ARGB(0, 255, 255, 255)
#define ALPHA_MASK	ARGB(255, 0, 0, 0)
#define BLACK		RGB(0, 0, 0)
#define WHITE		RGB(255, 255, 255)
#define RED		RGB(255, 0, 0)
#define GREEN		RGB(0, 255, 0)
#define BLUE		RGB(0, 0, 255)
#define GREY		RGB(0x77, 0x77, 0x77) //GW Gray shade

void ClearScreen(u8 *screen, u32 color);
void DrawCharacter(u8 *screen, char character, u32 x, u32 y, u32 color, u32 bgcolor);
void DrawString(u8 *screen, const char *str, u32 x, u32 y, u32 color, u32 bgcolor);
void DrawPixel(u8 *screen, u32 x, u32 y, u32 color);
u32 GetPixel(u8 *screen, u32 x, u32 y);
void Debug(const char *format, ...);

void SplashScreen();
//Unused functions.
void DrawHex(u8 *screen, u32 hex, u32 x, u32 y, u32 color, u32 bgcolor);
void DrawHexWithName(u8 *screen, const char *str, u32 hex, u32 x, u32 y, u32 color, u32 bgcolor);
