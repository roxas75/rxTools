/*
 * Copyright (C) 2015-2016 The PASTA Team
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

#include <stddef.h>
#include <stdint.h>
#include <lib/fatfs/ff.h>
#include <lib/ui/draw.h>
#include <lib/fs.h>

__attribute__((section(".text.cstart"), noreturn)) void initStart()
{
	const uintptr_t p = 0x23E00000;
	const wchar_t *msg;
	UINT br;
	int r;
	FIL f;

	// Enable TMIO IRQ
	*(volatile uint32_t *)0x10001000 = 0x00010000;

	// Unknown register to fix mounting of SDMC
	*(volatile uint32_t *)0x10000020 = 0x340;

	if (!FSInit()) {
		msg = L"Failed to initialize FS";
		goto fatal;
	}

	r = f_open(&f, L"" SYS_PATH "/tool.bin", FA_READ);
	if (r != FR_OK) {
		msg = L"Failed to open tool";
		goto fatal;
	}

	r = f_read(&f, (void *)p, 0x100000, &br);
	if (r != FR_OK) {
		msg = L"Failed to read tool";
		goto fatal;
	}

	f_close(&f);

	((void (*)())p)();
	__builtin_unreachable();

fatal:
	DrawString(BOT_SCREEN, msg,
		BOT_SCREEN_WIDTH / 2, SCREEN_HEIGHT - FONT_HEIGHT, RED, BLACK);
	while (1);
}
