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

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <reboot.h>
#include <features/TitleKeyDecrypt.h>
#include <features/install.h>
#include <lib/cfg.h>
#include <lib/lang.h>
#include <features/screenshot.h>
#include <lib/fs.h>
#include <lib/fatfs/ff.h>
#include <lib/console.h>
#include <lib/draw.h>
#include <lib/hid.h>
#include <lib/mpcore.h>
#include <lib/ncch.h>
#include <lib/crypto.h>
#include <lib/polarssl/aes.h>
#include <features/firm.h>
#include <features/downgradeapp.h>
#include "stdio.h"
#include <lib/menu.h>

#define DATA_PATH	_T("rxtools/data")
#define KEYFILENAME	"slot0x25KeyX.bin"

static FRESULT saveFirm(uint32_t id, const void *p, DWORD n)
{
	TCHAR path[64];
	UINT bw;
	FRESULT r;
	FIL f;

	getFirmPath(path, id);
	r = f_open(&f, path, FA_WRITE | FA_CREATE_ALWAYS);
	if (r != FR_OK)
		return r;

	r = f_write(&f, p, n, &bw);
	f_close(&f);

	return r;
}

static int processFirmFile(uint32_t lo)
{
	static const wchar_t pathFmt[] = L"rxTools/firm/00040138%08" PRIX32 "%ls.bin";
	const uint32_t hi = 0x00040138;
	uint8_t key[AES_BLOCK_SIZE];
	wchar_t path[_MAX_LFN];
	void *buff, *firm;
	UINT size;
	FRESULT r;
	FIL f;

	swprintf(path, _MAX_LFN, pathFmt, lo, "");
	r = f_open(&f, path, FA_READ);
	if (r != FR_OK)
		return r;

	size = f_size(&f);
	buff = __builtin_alloca(size + sizeof(uint32_t));
	r = f_read(&f, buff, size, &size);
	f_close(&f);
	if (r != FR_OK)
		return r;

	swprintf(path, _MAX_LFN, pathFmt, lo, L"_cetk");
	if (!getTitleKeyWithCetk(key, path)) {
		firm = decryptFirmTitle(buff, size, &size, key);
		if (firm != NULL)
			return saveFirm(lo, firm, size);
	}

	if (!getTitleKey(key, hi, lo, 1)) {
		firm = decryptFirmTitle(buff, size, &size, key);
		if (firm != NULL)
			return saveFirm(lo, firm, size);
	}

	if (!getTitleKey(key, hi, lo, 2)) {
		firm = decryptFirmTitle(buff, size, &size, key);
		if (firm != NULL)
			return saveFirm(lo, firm, size);
	}

	return -1;
}

static int processFirmInstalled(uint32_t lo)
{
	void *buff, *firm;
	AppInfo appInfo;
	UINT size;
	FRESULT r;
	FIL f;

	appInfo.drive = 1;
	appInfo.tidLo = lo;
	appInfo.tidHi = TID_HI_FIRM;
	FindApp(&appInfo);
	if (f_open(&f, appInfo.content, FA_READ) != FR_OK) {
		appInfo.drive = 2;
		FindApp(&appInfo);
		r = f_open(&f, appInfo.content, FA_READ);
		if (r != FR_OK)
			return r;
	}

	size = f_size(&f);
	buff = __builtin_alloca(size);

	r = f_read(&f, buff, size, &size);
	f_close(&f);
	if (r != FR_OK)
		return r;

	firm = decryptFirmTitleNcch(buff, &size);
	return firm == NULL ? -1 : saveFirm(lo, firm, size);
}

static int processFirm(uint32_t lo)
{
	int r;

	r = processFirmFile(lo);
	if (r && processFirmInstalled(lo))
		return r;

	return 0;
}

typedef struct {
	wchar_t str[16];
	wchar_t *cur;
	unsigned int x;
} Bar;

static void initBar(Bar *b, size_t n)
{
	const wchar_t *src;
	wchar_t *dst;

	b->cur = b->str;
	b->x = (BOT_SCREEN_WIDTH - n * FONT_WIDTH) / 2;

	dst = b->str;
	while (n > 0) {
		for (src = strings[STR_PROGRESS]; *src != 0; src++) {
			*dst = *src;
			dst++;
		}

		n--;
	}

	*dst = 0;
	DrawString(BOT_SCREEN, b->str, b->x, 50,
		ConsoleGetTextColor(), ConsoleGetBackgroundColor());
}

static void incBar(Bar *b)
{
	const wchar_t *p;

	for (p = strings[STR_PROGRESS_OK]; *p != 0; p++) {
		*b->cur = *p;
		b->cur++;
	}

	DrawString(BOT_SCREEN, b->str, b->x, 50,
		ConsoleGetTextColor(), ConsoleGetBackgroundColor());
}

static int InstallData()
{
	static TCHAR date[] = DATA_PATH "/data.bin";
	Bar b;
	int r;

	initBar(&b, getMpInfo() == MPINFO_CTR ? 5 : 3);

	f_mkdir(DATA_PATH);
	incBar(&b);

	r = processFirm(getMpInfo() == MPINFO_CTR ?
		TID_CTR_NATIVE_FIRM : TID_KTR_NATIVE_FIRM);
	if (r)
		return r;

	incBar(&b);

	if (getMpInfo() == MPINFO_CTR) {
		r = processFirm(TID_CTR_AGB_FIRM);
		if (r)
			return r;

		incBar(&b);

		r = processFirm(TID_CTR_TWL_FIRM);
		if (r != FR_OK)
			return r;

		incBar(&b);
	}

	if (f_stat(date, NULL) == FR_OK)
		f_unlink(date);

	incBar(&b);

	return 0;
}

int CheckInstallationData(){
	File file;
	TCHAR str[64];

	switch (getMpInfo()) {
		case MPINFO_CTR:
			getFirmPath(str, TID_CTR_NATIVE_FIRM);
			if(!FileOpen(&file, str, 0)) return -1;
			FileClose(&file);

			getFirmPath(str, TID_CTR_TWL_FIRM);
			if(!FileOpen(&file, str, 0)) return -2;
			FileClose(&file);

			getFirmPath(str, TID_CTR_AGB_FIRM);
			if(!FileOpen(&file, str, 0)) return -3;
			FileClose(&file);

			break;

		case MPINFO_KTR:
			getFirmPath(str, TID_KTR_NATIVE_FIRM);
			if(!FileOpen(&file, str, 0)) return -1;
			FileClose(&file);

			break;

		default:
			return 0;
	}

	if (f_stat(L"rxTools/data/data.bin", 0) == FR_OK)
		return -4;

	return 0;
}

void InstallConfigData(){
	wchar_t path[_MAX_LFN], pathL[_MAX_LFN], pathR[_MAX_LFN];

	if(CheckInstallationData() == 0)
		return;

	trySetLangFromTheme(0);
	writeCfg();

	swprintf(path, _MAX_LFN, L"/rxTools/Theme/%u/cfg0TOP.bin",
		cfgs[CFG_THEME].val.i);
	DrawTopSplash(path, path, path);
	swprintf(path, _MAX_LFN, L"/rxTools/Theme/%u/cfg0.bin",
		cfgs[CFG_THEME].val.i);
	DrawBottomSplash(path);

	int res = InstallData();
	swprintf(path, _MAX_LFN, L"/rxTools/Theme/%u/cfg1%c.bin",
		cfgs[CFG_THEME].val.i, res == 0 ? 'O' : 'E');
	DrawBottomSplash(path);
	swprintf(path, _MAX_LFN, L"/rxTools/Theme/%u/TOP.bin",
		cfgs[CFG_THEME].val.i);
	swprintf(pathL, _MAX_LFN, L"/rxTools/Theme/%u/TOPL.bin",
		cfgs[CFG_THEME].val.i);
	swprintf(pathR, _MAX_LFN, L"/rxTools/Theme/%u/TOPR.bin",
		cfgs[CFG_THEME].val.i);
	DrawTopSplash(path, pathL, pathR);

	InputWait();
}
