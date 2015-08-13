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

#include "draw.h"

#ifndef CONSOLE_H
#define CONSOLE_H

#include <wchar.h>

#define CONSOLE_SIZE			0x4000
#define CONSOLE_WIDTH			BOT_SCREEN_WIDTH - FONT_WIDTH * 2
#define CONSOLE_HEIGHT			SCREEN_HEIGHT - FONT_HEIGHT * 3
#define CONSOLE_MAX_LINES		(int)(CONSOLE_HEIGHT / FONT_HEIGHT - 4)
#define CONSOLE_MAX_LINE_LENGTH		(int)(CONSOLE_WIDTH / FONT_HWIDTH - 4)
#define CONSOLE_MAX_TITLE_LENGTH	(int)(BOT_SCREEN_WIDTH / FONT_HWIDTH)
#define CONSOLE_X			(BOT_SCREEN_WIDTH - CONSOLE_WIDTH) / 2
#define CONSOLE_Y			(SCREEN_HEIGHT - CONSOLE_HEIGHT) / 2

void ConsoleInit();
void ConsoleShow();
void ConsoleFlush();
void ConsoleAddText(wchar_t* str);
void ConsoleSetBackgroundColor(int color);
int ConsoleGetBackgroundColor();
void ConsoleSetBorderColor(int color);
int ConsoleGetBorderColor();
void ConsoleSetTextColor(int color);
int ConsoleGetTextColor();
void ConsoleSetXY(int x, int y);
void ConsoleGetXY(int *x, int *y);
void ConsoleSetWH(int width, int height);
void ConsoleSetTitle(const wchar_t *format, ...);
void ConsoleNextLine();
void ConsolePrevLine();
void ConsoleNext();
void ConsolePrev();
void ConsoleSetBorderWidth(int width);
int ConsoleGetBorderWidth(int width);
void ConsoleSetSpecialColor(int color);
int ConsoleGetSpecialColor();
void ConsoleSetSpacing(int space);
int ConsoleGetSpacing();

void print(const wchar_t *format, ...);
#endif
