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

	[STR_FORCE_UI_BOOT] = "FORCE_UI_BOOT",
	[STR_SELECTED_THEME] = "SELECTED_THEME",
	[STR_SHOW_AGB] = "SHOW_AGB",
	[STR_ENABLE_3D_UI] = "ENABLE_3D_UI",
	[STR_QUICK_BOOT] = "QUICK_BOOT",
	[STR_CONSOLE_LANGUAGE] = "CONSOLE_LANGUAGE",
	[STR_AUTOBOOT] = "AUTOBOOT",
	[STR_HOLD_R] = "HOLD_R"
};

int loadStrings()
{
	const size_t tokenNum = 1 + STR_NUM * 2;
	jsmntok_t t[tokenNum];
	char buf[1024];
	jsmn_parser p;
	unsigned int i, j, k, l;
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
				len = t[i].end - t[i].start;
				for (s = buf + t[i].start; len > 0
					&& l < STR_MAX_LEN; s += k)
				{
					k = mbtowc(strings[j][l], s, len);
					if (k < 0)
						break;

					l++;
					len -= k;
				}

				strings[j][l] = 0;
				break;
			}
		}
	}

	return 0;
}
