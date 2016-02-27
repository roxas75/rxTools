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
#include <string.h>
#include <lib/fatfs/ff.h>
#include <lib/cfg.h>
#include <lib/fs.h>
#include <lib/lang.h>
#include "jsmn/jsmn.h"

static char cfgLang[CFG_STR_MAX_LEN] = "en.json";

Cfg cfgs[] = {
	[CFG_GUI] = { "GUI", CFG_TYPE_BOOLEAN, { .i = 0 } },
	[CFG_THEME] = { "Theme", CFG_TYPE_INT, { .i = 0 } },
	[CFG_RANDOM] = { "Random theme", CFG_TYPE_BOOLEAN, { .i = 0 } },
	[CFG_AGB] = { "AGB", CFG_TYPE_BOOLEAN, { .i = 0 } },
	[CFG_3D] = { "3D", CFG_TYPE_BOOLEAN, { .i = 1 } },
	[CFG_ABSYSN] = { "Autoboot-sysNAND", CFG_TYPE_BOOLEAN, { .i = 0 } },
	[CFG_LANG] = { "Language", CFG_TYPE_STRING, { .s = cfgLang } }
};

static const TCHAR jsonPath[] = _T("/rxTools/data/system.json");

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int writeCfg()
{
	File fd;
	char buf[256];
	const char *p;
	char *jsonCur;
	unsigned int i;
	size_t len;
	int left, res;

	left = sizeof(buf);
	jsonCur = buf;

	*jsonCur = '{';

	left--;
	jsonCur++;

	i = 0;
	for (i = 0; i < CFG_NUM; i++) {
		if (i > 0) {
			if (left < 1)
				return 1;

			*jsonCur = ',';
			left--;
			jsonCur++;
		}

		res = snprintf(jsonCur, left, "\n\t\"%s\": ", cfgs[i].key);
		if (res < 0 || res >= left)
			return 1;

		left -= res;
		jsonCur += res;

		switch (cfgs[i].type) {
			case CFG_TYPE_INT:
				res = snprintf(jsonCur, left,
					"%d", cfgs[i].val.i);
				if (res < 0 || res >= left)
					return 1;

				len = res;
				break;

			case CFG_TYPE_BOOLEAN:
				if (cfgs[i].val.i) {
					len = sizeof("true");
					p = "true";
				} else {
					len = sizeof("false");
					p = "false";
				}

				if (len >= left)
					return -1;

				strcpy(jsonCur, p);
				len--;
				break;

			case CFG_TYPE_STRING:
				res = snprintf(jsonCur, left,
					"\"%s\"", cfgs[i].val.s);
				if (res < 0 || res >= left)
					return 1;

				len = res;
				break;

			default:
				return -1;
		}

		left -= len;
		jsonCur += len;
	}

	left -= 3;
	if (left < 0)
		return 1;

	*jsonCur = '\n';
	jsonCur++;
	*jsonCur = '}';
	jsonCur++;
	*jsonCur = '\n';
	jsonCur++;

	if (!FileOpen(&fd, jsonPath, 1))
		return 1;

	FileWrite(&fd, buf, (uintptr_t)jsonCur - (uintptr_t)buf, 0);
	FileClose(&fd);

	return 0;
}

int readCfg()
{
	const size_t tokenNum = 1 + CFG_NUM * 2;
	jsmntok_t t[tokenNum];
	char buf[256];
	jsmn_parser parser;
	File fd;
	unsigned int i, j, k;
	int r;
	size_t len;

	if (!FileOpen(&fd, jsonPath, 0))
		return 1;

	len = FileGetSize(&fd);
	if (len > sizeof(buf))
		return 1;

	FileRead(&fd, buf, len, 0);
	FileClose(&fd);

	jsmn_init(&parser);
	r = jsmn_parse(&parser, buf, len, t, tokenNum);
	if (r < 0)
		return r;

	if (r < 1)
		return 1;

	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		for (j = 0; jsoneq(buf, &t[i], cfgs[j].key) != 0; j++)
			if (j >= CFG_NUM)
				return 1;

		i++;
		switch (cfgs[j].type) {
			case CFG_TYPE_INT:
				cfgs[j].val.i = 0;
				for (k = t[i].start; k < t[i].end; k++) {
					cfgs[j].val.i *= 10;
					cfgs[j].val.i += buf[k] - 48;
				}

				break;

			case CFG_TYPE_BOOLEAN:
				len = t[i].end - t[i].start;
				cfgs[j].val.i = buf[t[i].start] == 't';

				break;

			case CFG_TYPE_STRING:
				len = t[i].end - t[i].start;

				if (len + 1 > CFG_STR_MAX_LEN)
					break;

#ifdef DEBUG
				if (cfgs[j].val.s == NULL)
					break;
#endif

				memcpy(cfgs[j].val.s, buf + t[i].start, len);
				cfgs[j].val.s[len] = 0;
		}
	}

	return 0;
}

void trySetLangFromTheme(int onswitch) {
	File MyFile;
	wchar_t str[_MAX_LFN];
	unsigned int i;

	swprintf(str, _MAX_LFN, L"/rxTools/Theme/%u/LANG.txt",
		cfgs[CFG_THEME].val.i);
	if (!FileOpen(&MyFile, str, 0))
		return;
	if (FileGetSize(&MyFile) > 0)
	{
		FileRead(&MyFile, cfgs[CFG_LANG].val.s, CFG_STR_MAX_LEN, 0);

		for (i = 0; i + 1 < CFG_STR_MAX_LEN
			&& cfgs[CFG_LANG].val.s[i] != '\r'
			&& cfgs[CFG_LANG].val.s[i] != '\n'; i++);
		cfgs[CFG_LANG].val.s[i] = 0;

		if(onswitch)
			switchStrings();
	}
	FileClose(&MyFile);
}
