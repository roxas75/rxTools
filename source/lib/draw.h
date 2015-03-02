#pragma once

#include "common.h"
//Screen Macros
#define BYTES_PER_PIXEL 3
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 400
#define SCREEN_SIZE (BYTES_PER_PIXEL*SCREEN_WIDTH*SCREEN_HEIGHT)
#define TOP_SCREEN (u8*)(*(u32*)((uint32_t)0x080FFFC0 + 4 * (*(u32*)0x080FFFD8 & 1)))
#define BOT_SCREEN (u8*)(*(u32*)0x080FFFD4)

//Colors Macros
#define RGB(r,g,b) (r<<24|b<<16|g<<8|r)
#define BLACK 		RGB(0, 0, 0)
#define WHITE		RGB(255, 255, 255)
#define RED			RGB(255, 0, 0)
#define GREEN		RGB(0, 255, 0)
#define BLUE		RGB(0, 0, 255)
#define GREY		RGB(0x77, 0x77, 0x77)				 //GW Gray shade
#define TRANSPARENT RGB(255, 0, 255) 				//Magenta is my favourite transparent color

extern int current_y;

void ClearScreen(unsigned char *screen, int color);
void DrawCharacter(unsigned char *screen, int character, int x, int y, int color, int bgcolor);
void DrawHex(unsigned char *screen, unsigned int hex, int x, int y, int color, int bgcolor);
void DrawString(unsigned char *screen, const char *str, int x, int y, int color, int bgcolor);
void DrawHexWithName(unsigned char *screen, const char *str, unsigned int hex, int x, int y, int color, int bgcolor);
void DrawPixel(int x, int y, int color, int screen);
int GetPixel(int x, int y, int screen);
void Debug(const char *format, ...);

void SplashScreen();
