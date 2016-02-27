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
#include <lib/crypto.h>
#include <lib/fs.h>
#include <lib/ui/console.h>
#include <lib/ui/draw.h>
#include <lib/hid.h>
#include <lib/mpcore.h>
#include <features/screenshot.h>
#include <features/firm.h>
#include <features/install.h>
#include <lib/log.h>
#include <lib/i2c.h>
#include <lib/cfg.h>
#include <lib/lang.h>

#define FONT_NAME "font.bin"

static void ShutDown(int arg){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, (arg) ? (uint8_t)(1<<0):(uint8_t)(1<<2));
	while(1);
}

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
	f_mkdir(L"rxTools");
	f_mkdir(L"rxTools/nand");
	InstallConfigData();
}

static void drawTop()
{
	wchar_t str[_MAX_LFN];
	wchar_t strl[_MAX_LFN];
	wchar_t strr[_MAX_LFN];

	swprintf(str, _MAX_LFN, L"/rxTools/Theme/%u/TOP.bin",
		cfgs[CFG_THEME].val.i);
	if (cfgs[CFG_3D].val.i) {
		swprintf(strl, _MAX_LFN, L"/rxTools/Theme/%u/TOPL.bin",
			cfgs[CFG_THEME].val.i);
		swprintf(strr, _MAX_LFN, L"/rxTools/Theme/%u/TOPR.bin",
			cfgs[CFG_THEME].val.i);
		DrawTopSplash(str, strl, strr);
	} else
		DrawTopSplash(str, str, str);
}

static void drawBottom()
{
	wchar_t str[_MAX_LFN];

	swprintf(str, _MAX_LFN, L"/rxTools/Theme/%u/BOT%u.bin",
		cfgs[CFG_THEME].val.i, cfgs[CFG_DEFAULT].val.i);
	DrawBottomSplash(str);
}

static FRESULT initKeyX()
{
	uint8_t buff[AES_BLOCK_SIZE];
	UINT br;
	FRESULT r;
	FIL f;

	r = f_open(&f, _T("slot0x25KeyX.bin"), FA_READ);
	if (r != FR_OK)
		return r;

	r = f_read(&f, buff, sizeof(buff), &br);
	if (br < sizeof(buff))
		return r == FR_OK ? EOF : r;

	f_close(&f);
	aesSetKeyX(0x25, buff);
	return 0;
}

static FRESULT initN3DSKeys()
{
    uint8_t buff[AES_BLOCK_SIZE];
	UINT br;
	FRESULT r;
	FIL f;

	r = f_open(&f, _T("key_0x16.bin"), FA_READ);
	if (r != FR_OK)
		return r;

	r = f_read(&f, buff, sizeof(buff), &br);
	if (br < sizeof(buff))
		return r == FR_OK ? EOF : r;

	f_close(&f);
	aesSetKeyX(0x16, buff);

	r = f_open(&f, _T("key_0x1B.bin"), FA_READ);
	if (r != FR_OK)
		return r;

	r = f_read(&f, buff, sizeof(buff), &br);
	if (br < sizeof(buff))
		return r == FR_OK ? EOF : r;

	f_close(&f);
	aesSetKeyX(0x1B, buff);
	return 0;
}

static _Noreturn void mainLoop()
{
	uint32_t pad;

	while (true) {
		drawBottom();
		pad = InputWait();

		if (pad & BUTTON_A) rxMode(1);		//EMUNAND
		if (pad & BUTTON_X) rxMode(0);		//SYSNAND
		if (pad & BUTTON_Y) PastaMode();	//PASTAMODE
		if (pad & BUTTON_B) ShutDown(1);	//SHUTDOWN
		if (pad & BUTTON_LEFT)
		{
			if(cfgs[CFG_DEFAULT].val.i == 0) cfgs[CFG_DEFAULT].val.i = 3;
			else cfgs[CFG_DEFAULT].val.i--;
			writeCfg();

		}
		if(pad & BUTTON_RIGHT)
		{
			if(cfgs[CFG_DEFAULT].val.i == 3) cfgs[CFG_DEFAULT].val.i = 0;
			else cfgs[CFG_DEFAULT].val.i++;
			writeCfg();
		}
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
	static const TCHAR fontPath[] = _T("") SYS_PATH "/" FONT_NAME;
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

	
	set_loglevel(ll_info);
	log(ll_info, "Initializing rxTools...");
	

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

	if (getMpInfo() == MPINFO_KTR)
    {
        r = initN3DSKeys();
        if (r != FR_OK) {
            warn(L"Failed to load keys for N3DS\n"
            "  Code: %d\n"
            "  RxMode will not boot. Please\n"
            "  include key_0x16.bin and\n"
            "  key_0x1B.bin at the root of your\n"
            "  SD card.\n", r);
            InputWait();
            goto postinstall;
        }
    }


	install();
	postinstall:
	readCfg();

	log(ll_info, "Done...");

	r = loadStrings();
	if (r)
		warn(L"Failed to load strings: %d\n", r);

	drawTop();

	//Default boot check
	if (cfgs[CFG_DEFAULT].val.i && HID_STATE & BUTTON_L1)
		{
			if(cfgs[CFG_DEFAULT].val.i == 3) PastaMode();
			else rxMode(cfgs[CFG_DEFAULT].val.i - 1);
		}

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
	mainLoop();
}
