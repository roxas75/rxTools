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

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include "MainMenu.h"
#include <lib/crypto.h>
#include <lib/fs.h>
#include <lib/ui/console.h>
#include <lib/ui/draw.h>
#include <lib/hid.h>
#include <features/screenshot.h>
#include <features/firm.h>
#include <features/configuration.h>
#include <lib/log.h>

#define FONT_NAME "font.bin"

static int warned = 0;

static void setConsole()
{
	ConsoleSetXY(15, 20);
	ConsoleSetWH(BOT_SCREEN_WIDTH - 30, SCREEN_HEIGHT - 80);
	ConsoleSetBorderColor(BLUE);
	ConsoleSetTextColor(RGB(0, 141, 197));
	ConsoleSetBackgroundColor(TRANSPARENT);
	ConsoleSetSpecialColor(BLUE);
	ConsoleSetSpacing(2);
	ConsoleSetBorderWidth(3);
}

static void install()
{
	f_mkdir("rxTools");
	f_mkdir("rxTools/nand");
	InstallConfigData();
}

static void drawTop()
{
	char str[64];
	char strl[64];
	char strr[64];

	sprintf(str, "/rxTools/Theme/%u/TOP.bin", cfgs[CFG_THEME].val.i);
	if (cfgs[CFG_3D].val.i) {
		sprintf(strl, "/rxTools/Theme/%u/TOPL.bin", cfgs[CFG_THEME].val.i);
		sprintf(strr, "/rxTools/Theme/%u/TOPR.bin", cfgs[CFG_THEME].val.i);
		DrawTopSplash(str, strl, strr);
	} else
		DrawTopSplash(str, str, str);
}

static FRESULT initKeyX()
{
	uint8_t buff[AES_BLOCK_SIZE];
	UINT br;
	FRESULT r;
	FIL f;

	r = f_open(&f, "slot0x25KeyX.bin", FA_READ);
	if (r != FR_OK)
		return r;

	r = f_read(&f, buff, sizeof(buff), &br);
	if (br < sizeof(buff))
		return r == FR_OK ? EOF : r;

	f_close(&f);
	aesSetKeyX(0x25, buff);
	return 0;
}

static _Noreturn void mainLoop()
{
	uint32_t pad;

	while (true) {
		pad = InputWait();
		if (pad & (BUTTON_DOWN | BUTTON_RIGHT | BUTTON_R1))
			MenuNextSelection();

		if (pad & (BUTTON_UP | BUTTON_LEFT | BUTTON_L1))
			MenuPrevSelection();

		if (pad & BUTTON_A) {
			OpenAnimation();
			MenuSelect();
		}

		if (pad & BUTTON_SELECT) {
			fadeOut();
			ShutDown();
		}

		MenuShow();
	}
}

static void warn(const wchar_t *format, ...)
{
	va_list va;

	if (!warned) {
		ConsoleInit();
		ConsoleSetTitle(strings[STR_WARNING]);
		warned = 1;
	}

	va_start(va, format);
	vprint(format, va);
	va_end(va);

	ConsoleShow();
}

__attribute__((section(".text.start"), noreturn)) void _start()
{
	static const char fontPath[] = SYS_PATH "/" FONT_NAME;
	void *fontBuf;
	UINT btr, br;
	int r;
	FIL f;

	// Enable TMIO IRQ
	*(volatile uint32_t *)0x10001000 = 0x00010000;

	preloadStringsA();

	if (!FSInit()) {
		DrawString(BOT_SCREEN, strings[STR_FAILED],
			BOT_SCREEN_WIDTH / 2, SCREEN_HEIGHT - FONT_HEIGHT, RED, BLACK);
		while (1);
	}

	/*
	set_loglevel(ll_info);
	log(ll_info, "Initializing rxTools...");
	*/

	setConsole();

	fontIsLoaded = 0;
	r = f_open(&f, fontPath, FA_READ);
	if (r == FR_OK) {
		btr = f_size(&f);
		fontBuf = __builtin_alloca(btr);
		r = f_read(&f, fontBuf, btr, &br);
		if (r == FR_OK)
			fontIsLoaded = 1;

		f_close(&f);
		fontaddr = fontBuf;
	}

	if (fontIsLoaded)
		preloadStringsU();
	else
		warn(L"Failed to load " FONT_NAME ": %d\n", r);

	install();
	readCfg();

	r = loadStrings();
	if (r)
		warn(L"Failed to load strings: %d\n", r);

	drawTop();

	if (!cfgs[CFG_GUI].val.i && HID_STATE & BUTTON_L1)
		rxMode(cfgs[CFG_ABSYSN].val.i ? 0 : 1);

	if (sysver < 7) {
		r = initKeyX();
		if (r != FR_OK)
			warn(L"Failed to load key X for slot 0x25\n"
				"  Code: %d\n"
				"  If your firmware version is less\n"
				"  than 7.X, some titles decryption\n"
				"  will fail, and some EmuNANDs\n"
				"  will not boot.\n", r);
	}

	if (warned) {
		warn(strings[STR_PRESS_BUTTON_ACTION],
			strings[STR_BUTTON_A], strings[STR_CONTINUE]);
		WaitForButton(BUTTON_A);
	}

	OpenAnimation();
	MenuInit(&MainMenu);
	MenuShow();
	mainLoop();
}
