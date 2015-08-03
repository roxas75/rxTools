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

#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include "fatfs/ff.h"
#include "fs.h"
#include "jsmn.h"
#include "lang.h"

wchar_t strings[STR_NUM][STR_MAX_LEN];
const char langPath[] = "/rxTools/lang";

static const char *keys[STR_NUM] = {
	[STR_LANG_NAME] = "LANG_NAME",
	[STR_DUMP] = "DUMP",
	[STR_DUMPING] = "DUMPING",
	[STR_INJECT] = "INJECT",
	[STR_CHOOSE] = "CHOOSE",
	[STR_NAND] = "NAND",
	[STR_SYSNAND] = "SYSNAND",
	[STR_EMUNAND] = "EMUNAND",
	[STR_BUTTON_ACTION] = "BUTTON_ACTION",
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
	[STR_FAILED] = "FAILED",
	[STR_COMPLETED] = "COMPLETED",
	[STR_SETTINGS] = "SETTINGS",
	[STR_CHOOSE_NAND] = "CHOOSE_NAND",
	[STR_PRESS_X_SYSNAND] = "PRESS_X_SYSNAND",
	[STR_PRESS_Y_EMUNAND] = "PRESS_Y_EMUNAND",
	[STR_PRESS_B_BACK] = "PRESS_B_BACK",
	[STR_MISSING_THEME_FILES] = "MISSING_THEME_FILES",
	[STR_FORCE_UI_BOOT] = "FORCE_UI_BOOT",
	[STR_SELECTED_THEME] = "SELECTED_THEME",
	[STR_SHOW_AGB] = "SHOW_AGB",
	[STR_ENABLE_3D_UI] = "ENABLE_3D_UI",
	[STR_QUICK_BOOT] = "QUICK_BOOT",
	[STR_CONSOLE_LANGUAGE] = "CONSOLE_LANGUAGE",
	[STR_AUTOBOOT] = "AUTOBOOT",
	[STR_HOLD_R] = "HOLD_R",
	[STR_YES] = "YES",
	[STR_NO] = "NO"
};

int loadStrings()
{
	const size_t tokenNum = 1 + STR_NUM * 2;
	jsmntok_t t[tokenNum];
	char buf[10240];
	jsmn_parser p;
	unsigned int i, j, k, l=0;
	const char *s;
	int r;
	size_t len;
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
			if (!memcmp(s, keys[j], len)) {
				i++;
				strings[j][mbstowcs(strings[j], buf + t[i].start, t[i].end - t[i].start)]=0;
/*				len = t[i].end - t[i].start;
				for (s = buf + t[i].start; len > 0
					&& l < STR_MAX_LEN; s += k)
				{
					k = mbtowc((wchar_t*)strings[j][l], s, len);
					if (k < 0)
						break;

					l++;
					len -= k;
				}

				strings[j][l] = 0;
*/				break;
			}
		}
	}

	return 0;
}
