#pragma once

#include "common.h"
//Screen Macros
#define BYTES_PER_PIXEL     3  //Our color buffer accepts 24-bits color.
#define SCREEN_WIDTH        400
#define SCREEN_HEIGHT       240
#define SCREEN_SIZE         (BYTES_PER_PIXEL*SCREEN_WIDTH*SCREEN_HEIGHT)
#define TOP_SCREEN          (u8*)(*(u32*)((uint32_t)0x080FFFC0 + 4 * (*(u32*)0x080FFFD8 & 1)))
#define TOP_SCREEN2         (u8*)(*(u32*)0x080FFFC4)
#define BOT_SCREEN          (u8*)(*(u32*)0x080FFFD4)

//Colors Macros
#define RGB(r,g,b)          (r<<16|g<<8|b)          //console asks for B,G,R in bytes
#define BLACK               RGB(0, 0, 0)
#define WHITE               RGB(255, 255, 255)
#define RED                 RGB(255, 0, 0)
#define GREEN	    	    RGB(0, 255, 0)
#define BLUE		        RGB(0, 0, 255)
#define GREY		        RGB(0x77, 0x77, 0x77)   //GW Gray shade
#define TRANSPARENT         RGB(255, 0, 255)        //Magenta is my favourite transparent color

void ClearScreen(unsigned char *screen, int color);
void DrawCharacter(unsigned char *screen, int character, int x, int y, int color, int bgcolor);
void DrawString(unsigned char *screen, const char *str, int x, int y, int color, int bgcolor);
void DrawPixel(int x, int y, int color, int screen);
int GetPixel(int x, int y, int screen);
void Debug(const char *format, ...);

void SplashScreen();
//Unused functions.
void DrawHex(unsigned char *screen, unsigned int hex, int x, int y, int color, int bgcolor);
void DrawHexWithName(unsigned char *screen, const char *str, unsigned int hex, int x, int y, int color, int bgcolor);
