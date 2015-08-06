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

#pragma once

#include "common.h"

//Screen Macros
#define BYTES_PER_PIXEL	3  //Our color buffer accepts 24-bits color.
#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240
#define SCREEN_SIZE	(BYTES_PER_PIXEL*SCREEN_WIDTH*SCREEN_HEIGHT)
#define FONT_WIDTH	16
#define FONT_HEIGHT	16
#define FONT_HWIDTH	(FONT_WIDTH>>1)
#define FONT_CJK_START	0x2400
#define FONT_PIXEL_PER_BYTE	8
#define TOP_SCREEN	(u8*)(*(u32*)0x080FFFC0)
#define TOP_SCREEN2	(u8*)(*(u32*)0x080FFFC4)
#define BOT_SCREEN	(u8*)(*(u32*)0x080FFFD4)
#define BOT_SCREEN2	(u8*)(*(u32*)0x080FFFD0)

//Colors Macros
#define ARGB(a,r,g,b)	(a<<24|r<<16|g<<8|b) //console asks for B,G,R in bytes
#define RGB(r,g,b)	ARGB(255,r,g,b) //opaque color
#define COLOR_MASK	ARGB(0, 255, 255, 255)
#define ALPHA_MASK	ARGB(255, 0, 0, 0)
#define TRANSPARENT	ARGB(0, 0, 0, 0)
#define BLACK		RGB(0, 0, 0)
#define WHITE		RGB(255, 255, 255)
#define RED		RGB(255, 0, 0)
#define GREEN		RGB(0, 255, 0)
#define BLUE		RGB(0, 0, 255)
#define GREY		RGB(0x77, 0x77, 0x77) //GW Gray shade

//Unicode special characters
#define CHAR_BUTTON_A	"\u2496"
#define CHAR_BUTTON_B	"\u2497"
#define CHAR_BUTTON_L	"\u24C1"
#define CHAR_BUTTON_R	"\u24C7"
#define CHAR_BUTTON_X	"\u24CD"
#define CHAR_BUTTON_Y	"\u24CE"
#define CHAR_SELECTED	"\u2714"
#define CHAR_UNSELECTED	"\u2718"

extern const u8 (* fontaddr)[FONT_WIDTH][FONT_HEIGHT / FONT_PIXEL_PER_BYTE];

void ClearScreen(u8 *screen, u32 color);
void DrawClearScreenAll(void);
void DrawCharacter(u8 *screen, wchar_t character, u32 x, u32 y, u32 color, u32 bgcolor);
void DrawString(u8 *screen, const wchar_t *str, u32 x, u32 y, u32 color, u32 bgcolor);
void DrawPixel(u8 *screen, u32 x, u32 y, u32 color);
u32 GetPixel(u8 *screen, u32 x, u32 y);
void Debug(const char *format, ...);

void DrawSplash(u8 *screen, char splash_file[]);
void DrawBottomSplash(char splash_file[]);
void DrawTopSplash(char splash_file[], char splash_fileL[], char splash_fileR[]);
void SplashScreen();
//Unused functions.
void DrawHex(u8 *screen, u32 hex, u32 x, u32 y, u32 color, u32 bgcolor);
void DrawHexWithName(u8 *screen, const wchar_t *str, u32 hex, u32 x, u32 y, u32 color, u32 bgcolor);
