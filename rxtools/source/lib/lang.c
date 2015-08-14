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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "fatfs/ff.h"
#include "fs.h"
#include "jsmn.h"
#include "lang.h"

wchar_t strings[STR_NUM][STR_MAX_LEN];
const char langPath[] = "/rxTools/lang";

static const char *keys[STR_NUM] = {
	[STR_LANG_NAME] = "LANG_NAME",
	[STR_BACKING_UP] = "BACKING_UP",
	[STR_DELETING] = "DELETING",
	[STR_DOWNGRADE] = "DOWNGRADE",
	[STR_DOWNGRADING] = "DOWNGRADING",
	[STR_DOWNGRADING_NOT_NEEDED] = "DOWNGRADING_NOT_NEEDED",
	[STR_DUMP] = "DUMP",
	[STR_DUMPING] = "DUMPING",
	[STR_INJECT] = "INJECT",
	[STR_INJECTING] = "INJECTING",
	[STR_DECRYPT] = "DECRYPT",
	[STR_DECRYPTING] = "DECRYPTING",
	[STR_DECRYPTING_TO] = "DECRYPTING_TO",
	[STR_DECRYPTED] = "DECRYPTED",
	[STR_GENERATE] = "GENERATE",
	[STR_GENERATING] = "GENERATING",
	[STR_PROCESSING] = "PROCESSING",
	[STR_OPENING] = "OPENING",
	[STR_RESTORE] = "RESTORE",
	[STR_RESTORING] = "RESTORING",
	[STR_LOAD] = "LOAD",
	[STR_DIRECTORY] = "DIRECTORY",
	[STR_MISSING] = "MISSING",
	[STR_ERROR_OPENING] = "ERROR_OPENING",
	[STR_ERROR_CREATING] = "ERROR_CREATING",
	[STR_ERROR_READING] = "ERROR_READING",
	[STR_ERROR_WRITING] = "ERROR_WRITING",
	[STR_ERROR_COPYING] = "ERROR_COPYING",
	[STR_ERROR_LAUNCHING] = "ERROR_LAUNCHING",
	[STR_WRONG] = "WRONG",
	[STR_GOT] = "GOT",
	[STR_EXPECTED] = "EXPECTED",
	[STR_VERSION] = "VERSION",
	[STR_VERSION_OF] = "VERSION_OF",
	[STR_ENTRIES_COUNT] = "ENTRIES_COUNT",
	[STR_SIZE] = "SIZE",
	[STR_CONTENT] = "CONTENT",
	[STR_CRYPTO_TYPE] = "CRYPTO_TYPE",
	[STR_CHOOSE] = "CHOOSE",
	[STR_NAND] = "NAND",
	[STR_SYSNAND] = "SYSNAND",
	[STR_EMUNAND] = "EMUNAND",
	[STR_NAND_PARTITIONS] = "NAND_PARTITIONS",
	[STR_NAND_XORPAD] = "NAND_XORPAD",
	[STR_XORPAD] = "XORPAD",
	[STR_EXHEADER] = "EXHEADER",
	[STR_EXEFS] = "EXEFS",
	[STR_ROMFS] = "ROMFS",
	[STR_TITLE_KEYS] = "TITLE_KEYS",
	[STR_TITLE_KEYS_FILE] = "TITLE_KEYS_FILE",
	[STR_FILES] = "FILES",
	[STR_PAD] = "PAD",
	[STR_DOWNGRADE_PACK] = "DOWNGRADE_PACK",
	[STR_FBI] = "FBI",
	[STR_HEALTH_AND_SAFETY] = "HEALTH_AND_SAFETY",
	[STR_TMD_VERSION] = "TMD_VERSION",
	[STR_TMD_SIZE] = "TMD_SIZE",
	[STR_HASH] = "HASH",
	[STR_FIRMWARE_FILE] = "FIRMWARE_FILE",
	[STR_CHECK_TMD_ONLY] = "CHECK_TMD_ONLY",
	[STR_INJECT_FBI] = "INJECT_FBI",
	[STR_SOURCE_ACTION] = "SOURCE_ACTION",
	[STR_SYSTEM_TITLES] = "SYSTEM_TITLES",
	[STR_SYSTEM_TITLES_WARNING] = "SYSTEM_TITLES_WARNING",
	[STR_SYSTEM_TITLES_DECRYPT] = "SYSTEM_TITLES_DECRYPT",
	[STR_KEYS_MISMATCH] = "KEYS_MISMATCH",
	[STR_NO_EMUNAND] = "NO_EMUNAND",
	[STR_TWLN] = "TWLN",
	[STR_TWLP] = "TWLP",
	[STR_AGB_SAVE] = "AGB_SAVE",
	[STR_FIRM0] = "FIRM0",
	[STR_FIRM1] = "FIRM1",
	[STR_CTRNAND] = "CTRNAND",
	[STR_CTR] = "CTR",
	[STR_TMD] = "TMD",
	[STR_KEY7] = "KEY7",
	[STR_SECURE] = "SECURE",
	[STR_NONE] = "NONE",
	[STR_JAPAN] = "JAPAN",
	[STR_USA] = "USA",
	[STR_EUROPE] = "EUROPE",
	[STR_CHINA] = "CHINA",
	[STR_KOREA] = "KOREA",
	[STR_TAIWAN] = "TAIWAN",
	[STR_BLANK_BUTTON_ACTION] = "BLANK_BUTTON_ACTION",
	[STR_PRESS_BUTTON_ACTION] = "PRESS_BUTTON_ACTION",
	[STR_HOLD_BUTTON_ACTION] = "HOLD_BUTTON_ACTION",
	[STR_BUTTON_A] = "BUTTON_A",
	[STR_BUTTON_B] = "BUTTON_B",
	[STR_BUTTON_X] = "BUTTON_X",
	[STR_BUTTON_Y] = "BUTTON_Y",
	[STR_BUTTON_L] = "BUTTON_L",
	[STR_BUTTON_R] = "BUTTON_R",
	[STR_BUTTON_ZL] = "BUTTON_ZL",
	[STR_BUTTON_ZR] = "BUTTON_ZR",
	[STR_BUTTON_SELECT] = "BUTTON_SELECT",
	[STR_BUTTON_HOME] = "BUTTON_HOME",
	[STR_BUTTON_START] = "BUTTON_START",
	[STR_CANCEL] = "CANCEL",
	[STR_CONTINUE] = "CONTINUE",
	[STR_KEEP] = "KEEP",
	[STR_DELETE] = "DELETE",
	[STR_OPEN_MENU] = "OPEN_MENU",
	[STR_FAILED] = "FAILED",
	[STR_COMPLETED] = "COMPLETED",
	[STR_SETTINGS] = "SETTINGS",
	[STR_FORCE_UI_BOOT] = "FORCE_UI_BOOT",
	[STR_SELECTED_THEME] = "SELECTED_THEME",
	[STR_SHOW_AGB] = "SHOW_AGB",
	[STR_ENABLE_3D_UI] = "ENABLE_3D_UI",
	[STR_QUICK_BOOT] = "QUICK_BOOT",
	[STR_ABSYSN] = "ABSYSN",
	[STR_MENU_LANGUAGE] = "MENU_LANGUAGE",
	[STR_AUTOBOOT] = "AUTOBOOT",
	[STR_INITIALIZING] = "INITIALIZING",
	[STR_LOADING] = "LOADING",
	[STR_WARNING] = "WARNING",
	[STR_WARNING_KEYFILE] = "WARNING_KEYFILE",
	[STR_CURSOR] = "CURSOR",
	[STR_NO_CURSOR] = "NO_CURSOR",
	[STR_ENABLED] = "ENABLED",
	[STR_DISABLED] = "DISABLED",
	[STR_PROGRESS] = "PROGRESS",
	[STR_PROGRESS_OK] = "PROGRESS_OK",
	[STR_PROGRESS_FAIL] = "PROGRESS_FAIL",
	[STR_REGION_] = "REGION_",
	[STR_REGION] = "REGION",
	[STR_MSET] = "MSET",
	[STR_MSET4] = "MSET4",
	[STR_MSET6] "MSET6",
	[STR_YES] = "YES",
	[STR_NO] = "NO"
};

void preloadStringsA()
{
//Strings used in initial loading and diagnostics
	wcscpy(strings[STR_INITIALIZING], L"Initializing...");
	wcscpy(strings[STR_LOADING], L"Loading...");
	wcscpy(strings[STR_FAILED], L"Failed!");
	wcscpy(strings[STR_ERROR_OPENING], L"Error opening %s!");
	wcscpy(strings[STR_AUTOBOOT], L"Autoboot");
	wcscpy(strings[STR_OPEN_MENU], L"open menu");
	wcscpy(strings[STR_WARNING], L"Warning");
	wcscpy(strings[STR_PRESS_BUTTON_ACTION], L"Press %ls to %ls\n");
	wcscpy(strings[STR_HOLD_BUTTON_ACTION], L"Hold %ls to %ls\n");
	wcscpy(strings[STR_CONTINUE], L"continue");
	wcscpy(strings[STR_WARNING_KEYFILE], L"If\nyour firmware version is less than\n7.X, some titles decryption will\nfail, and some EmuNANDs will not\nboot.\n");
//Strings with button names
	wcscpy(strings[STR_BUTTON_A], L"[A]");
	wcscpy(strings[STR_BUTTON_B], L"[B]");
	wcscpy(strings[STR_BUTTON_X], L"[X]");
	wcscpy(strings[STR_BUTTON_Y], L"[Y]");
	wcscpy(strings[STR_BUTTON_L], L"[L]");
	wcscpy(strings[STR_BUTTON_R], L"[R]");
	wcscpy(strings[STR_BUTTON_ZL], L"[ZL]");
	wcscpy(strings[STR_BUTTON_ZR], L"[ZR]");
	wcscpy(strings[STR_BUTTON_SELECT], L"[SELECT]");
	wcscpy(strings[STR_BUTTON_HOME], L"[HOME]");
	wcscpy(strings[STR_BUTTON_START], L"[START]");
//Special strings for interface elements
	wcscpy(strings[STR_CURSOR], L"=>");
	wcscpy(strings[STR_NO_CURSOR], L"  ");
	wcscpy(strings[STR_ENABLED], L" +");
	wcscpy(strings[STR_DISABLED], L" -");
	wcscpy(strings[STR_PROGRESS], L"--");
	wcscpy(strings[STR_PROGRESS_OK], L"++");
	wcscpy(strings[STR_PROGRESS_FAIL], L"**");
//Acronyms that most probably won't be translated
	wcscpy(strings[STR_MENU_LANGUAGE], L"Language  %16ls");
	wcscpy(strings[STR_BLANK_BUTTON_ACTION], L"%ls %ls\n");
	wcscpy(strings[STR_NAND], L"NAND");
	wcscpy(strings[STR_SYSNAND], L"sysNAND");
	wcscpy(strings[STR_EMUNAND], L"emuNAND");
	wcscpy(strings[STR_XORPAD], L"xorpad");
	wcscpy(strings[STR_NAND_XORPAD], L"NAND xorpad");
	wcscpy(strings[STR_EXHEADER], L"ExHeader");
	wcscpy(strings[STR_EXEFS], L"ExeFS");
	wcscpy(strings[STR_ROMFS], L"RomFS");
	wcscpy(strings[STR_TWLN], L"TWLN");
	wcscpy(strings[STR_TWLP], L"TWLP");
	wcscpy(strings[STR_AGB_SAVE], L"AGB_SAVE");
	wcscpy(strings[STR_FIRM0], L"FIRM0");
	wcscpy(strings[STR_FIRM1], L"FIRM1");
	wcscpy(strings[STR_CTRNAND], L"CTRNAND");
	wcscpy(strings[STR_CTR], L"CTR");
	wcscpy(strings[STR_TMD], L"TMD");
	wcscpy(strings[STR_JAPAN], L"Japan");
	wcscpy(strings[STR_USA], L"USA");
	wcscpy(strings[STR_EUROPE], L"Europe");
	wcscpy(strings[STR_CHINA], L"China");
	wcscpy(strings[STR_KOREA], L"Korea");
	wcscpy(strings[STR_TAIWAN], L"Taiwan");
	wcscpy(strings[STR_MSET], L"MSET");
	wcscpy(strings[STR_MSET4], L"MSET 4.x");
	wcscpy(strings[STR_MSET6], L"MSET 6.x");
	wcscpy(strings[STR_FBI], L"FBI");
}

void preloadStringsU()
{
//Strings with special characters available only with unicode font
	wcscpy(strings[STR_BUTTON_A], L"Ⓐ");
	wcscpy(strings[STR_BUTTON_B], L"Ⓑ");
	wcscpy(strings[STR_BUTTON_X], L"Ⓧ");
	wcscpy(strings[STR_BUTTON_Y], L"Ⓨ");
	wcscpy(strings[STR_CURSOR], L"⮞");
	wcscpy(strings[STR_NO_CURSOR], L"　");
	wcscpy(strings[STR_ENABLED], L"✔");
	wcscpy(strings[STR_DISABLED], L"✘");
//	wcscpy(strings[STR_ENABLED], L"⦿");
//	wcscpy(strings[STR_DISABLED], L"⦾");
	wcscpy(strings[STR_PROGRESS], L"⬜");
	wcscpy(strings[STR_PROGRESS_OK], L"⬛");
	wcscpy(strings[STR_PROGRESS_FAIL], L"✖");
}

void preloadStringsOnSwitch(void)
{
	extern int fontLoaded;
	preloadStringsA();
	if(!fontLoaded) preloadStringsU();
}

int loadStrings()
{
	const size_t tokenNum = 1 + STR_NUM * 2;
	jsmntok_t t[tokenNum];
	char buf[8192];
	jsmn_parser p;
	unsigned int i, j, k;
	const char *s;
	int l, r, len;
	File fd;

	sprintf(buf, "%s/%s", langPath, cfgs[CFG_LANG].val.s);
	if (!FileOpen(&fd, buf, 0))
		return 1;

	len = FileGetSize(&fd);
	if (len > sizeof(buf))
		return 1;

	FileRead(&fd, buf, len, 0);
	FileClose(&fd);

	jsmn_init(&p);
	r = jsmn_parse(&p, buf, len, t, tokenNum);
	if (r < 0)
		return r;

	for (i = 1; i < r; i++) {
		for (j = 0; j < STR_NUM; j++) {
			s = buf + t[i].start;

			len = t[i].end - t[i].start;
			if (!memcmp(s, keys[j], len) && !keys[j][len]) {
				i++;
				len = t[i].end - t[i].start;
				s = buf + t[i].start;
				for (k = 0; k + 1 < STR_MAX_LEN && len > 0; k++) {
					l = mbtowc(strings[j] + k, s, len);
					if (l < 0)
						break;

					len -= l;
					s += l;
				}

				strings[j][k] = 0;
				mbtowc(NULL, NULL, 0);
				break;
			}
		}
	}

	return 0;
}
