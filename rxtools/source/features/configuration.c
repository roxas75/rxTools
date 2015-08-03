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
#include <stdlib.h>
#include <stdio.h>
#include "configuration.h"
#include "lang.h"
#include "filepack.h"
#include "screenshot.h"
#include "fs.h"
#include "ff.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "ncch.h"
#include "crypto.h"
#include "aes.h"
#include "cfw.h"
#include "downgradeapp.h"
#include "stdio.h"
#include "menu.h"
#include "jsmn.h"

#define DATAFOLDER	"rxtools/data"
#define KEYFILENAME	"slot0x25KeyX.bin"
#define WORKBUF		(u8*)0x21000000
#define NAT_SIZE	0xEBC00
#define AGB_SIZE	0xD9C00
#define TWL_SIZE	0x1A1C00
#define PROGRESS_OK	L'⬛'
#define PROGRESS_FAIL	L'✖'
#define PROGRESS_X	(SCREEN_WIDTH-7*FONT_WIDTH)/2

bool first_boot;
char tmpstr[256] = {0};
char str[100];
char strl[100];
char strr[100];
File tempfile;
UINT tmpu32;

static char cfgLang[CFG_STR_MAX_LEN] = "en.json";

Cfg cfgs[] = {
	[CFG_GUI] = { "GUI", CFG_TYPE_BOOLEAN, { .i = 0 } },
	[CFG_THEME] = { "Theme", CFG_TYPE_INT, { .i = 0 } },
	[CFG_AGB] = { "AGB", CFG_TYPE_BOOLEAN, { .i = 0 } },
	[CFG_3D] = { "3D", CFG_TYPE_BOOLEAN, { .i = 1 } },
	[CFG_SILENT] = { "Silent", CFG_TYPE_BOOLEAN, { .i = 0 } },
	[CFG_LANG] = { "Language", CFG_TYPE_STRING, { .s = cfgLang } }
};

static const char jsonPath[] = "/rxTools/data/system.json";

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
	char buf[128];
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
	char buf[128];
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

int InstallData(char* drive){
	FIL firmfile;
	wchar_t *progressbar = L"⬜⬜⬜⬜⬜⬜⬜";
	wchar_t *progress = progressbar+0;
	print(L"%ls", progressbar);
	ConsolePrevLine();

	//Create the workdir
	sprintf(tmpstr, "%s:%s", drive, DATAFOLDER);
	f_mkdir(tmpstr);

	//Read firmware data
	if (f_open(&firmfile, "firmware.bin", FA_READ | FA_OPEN_EXISTING) != FR_OK) return CONF_NOFIRMBIN;
	*progress++ = PROGRESS_OK;
	DrawString(BOT_SCREEN, progressbar, PROGRESS_X, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());

	//Create patched native_firm
	f_read(&firmfile, WORKBUF, NAT_SIZE, &tmpu32);
	u8* n_firm = decryptFirmTitle(WORKBUF, NAT_SIZE, 0x00000002, 1);
	u8* n_firm_patch = GetFilePack("nat_patch.bin");
	applyPatch(n_firm, n_firm_patch);
	u8 keyx[16] = {0};
	if(GetSystemVersion() < 3){
		if (!FileOpen(&tempfile, KEYFILENAME, 0))
		{
			f_close(&firmfile);
			return CONF_CANTOPENFILE;
		}
		FileRead(&tempfile, &keyx[0], 16, 0);
		FileClose(&tempfile);
	}
	*progress++ = PROGRESS_OK;
	DrawString(BOT_SCREEN, progressbar, PROGRESS_X, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
	for(int i = 0; i < NAT_SIZE; i+=0x4){
		if(!strcmp((char*)n_firm + i, "InsertKeyXHere!") && keyx[0] != 0){
			memcpy(n_firm + i, keyx, 16);
		}
		if(*((unsigned int*)(n_firm + i)) == 0xAAAABBBB){
			*((unsigned int*)(n_firm + i)) = (checkEmuNAND() / 0x200) - 1;
		}
	}
	*progress++ = PROGRESS_OK;
	DrawString(BOT_SCREEN, progressbar, PROGRESS_X, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
	sprintf(tmpstr, "%s:%s/0004013800000002.bin", drive, DATAFOLDER);
	if(FileOpen(&tempfile, tmpstr, 1)){
		FileWrite(&tempfile, n_firm, NAT_SIZE, 0);
		FileClose(&tempfile);
	}else {
		f_close(&firmfile);
		return CONF_ERRNFIRM;
	}
	*progress++ = PROGRESS_OK;
	DrawString(BOT_SCREEN, progressbar, PROGRESS_X, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());

	//Create AGB patched firmware
	f_read(&firmfile, WORKBUF, AGB_SIZE, &tmpu32);
	u8* a_firm = decryptFirmTitle(WORKBUF, AGB_SIZE, 0x00000202, 1);
	u8* a_firm_patch = GetFilePack("agb_patch.bin");
	if (!a_firm && checkEmuNAND())
	{
		/* Try to get the Title Key from the EmuNAND */
		a_firm = decryptFirmTitle(WORKBUF, AGB_SIZE, 0x00000202, 2);
	}

	if(a_firm){
		applyPatch(a_firm, a_firm_patch);
		sprintf(tmpstr, "%s:%s/0004013800000202.bin", drive, DATAFOLDER);
		if(FileOpen(&tempfile, tmpstr, 1)){
			FileWrite(&tempfile, a_firm, AGB_SIZE, 0);
			FileClose(&tempfile);
		}else {
			f_close(&firmfile);
			return CONF_ERRNFIRM;
		}
		*progress++ = PROGRESS_OK;
	}else{
		//If we cannot decrypt it from firmware.bin because of titlekey messed up, it probably means that AGB has been modified in some way.
		//So we read it from his installed ncch...
		FindApp(0x00040138, 0x00000202, 1);
		char* path = getContentAppPath();
		if (!FileOpen(&tempfile, path, 0) && checkEmuNAND())
		{
			/* Try with EmuNAND */
			FindApp(0x00040138, 0x00000202, 2);
			path = getContentAppPath();
			if (!FileOpen(&tempfile, path, 0))
			{
				f_close(&firmfile);
				return CONF_ERRNFIRM;
			}
		}

		FileRead(&tempfile, WORKBUF, AGB_SIZE, 0);
		FileClose(&tempfile);
		a_firm = decryptFirmTitleNcch(WORKBUF, AGB_SIZE);
		if(a_firm){
			applyPatch(a_firm, a_firm_patch);
			sprintf(tmpstr, "%s:%s/0004013800000202.bin", drive, DATAFOLDER);
			if(FileOpen(&tempfile, tmpstr, 1)){
				FileWrite(&tempfile, a_firm, AGB_SIZE, 0);
				FileClose(&tempfile);
			}else {
				f_close(&firmfile);
				return CONF_ERRNFIRM;
			}
			*progress++ = PROGRESS_OK;
		}else{
			*progress++ = PROGRESS_FAIL; //If we get here, then we'll play without AGB, lol
		}
	}
	DrawString(BOT_SCREEN, progressbar, PROGRESS_X, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());

	//Create TWL patched firmware
	f_read(&firmfile, WORKBUF, TWL_SIZE, &tmpu32);
	u8* t_firm = decryptFirmTitle(WORKBUF, TWL_SIZE, 0x00000102, 1);
	u8* t_firm_patch = GetFilePack("twl_patch.bin");
	if(t_firm){
		applyPatch(t_firm, t_firm_patch);
		sprintf(tmpstr, "%s:%s/0004013800000102.bin", drive, DATAFOLDER);
		if(FileOpen(&tempfile, tmpstr, 1)){
			FileWrite(&tempfile, t_firm, TWL_SIZE, 0);
			FileClose(&tempfile);
			//FileCopy("0004013800000102.bin", tmpstr);
		}else {
			f_close(&firmfile);
			return CONF_ERRNFIRM;
		}
		*progress++ = PROGRESS_OK;
	}else{
		*progress++ = PROGRESS_FAIL;
	}
	DrawString(BOT_SCREEN, progressbar, PROGRESS_X, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());

	sprintf(tmpstr, "%s:%s/data.bin", drive, DATAFOLDER);
	if(FileOpen(&tempfile, tmpstr, 1)){
		FileWrite(&tempfile, __DATE__, 12, 0);
		FileWrite(&tempfile, __TIME__, 9, 12);
		FileClose(&tempfile);
	}else {
		f_close(&firmfile);
		return CONF_CANTOPENFILE;
	}
	*progress++ = PROGRESS_OK;
	DrawString(BOT_SCREEN, progressbar, PROGRESS_X, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());

	f_close(&firmfile);
	return 0;
}

int CheckInstallationData(){
	File file;
	char str[32];
	if(!FileOpen(&file, "rxTools/data/0004013800000002.bin", 0)) return -1;
	FileClose(&file);
	if(!FileOpen(&file, "rxTools/data/0004013800000202.bin", 0)) return -2;
	FileClose(&file);
	if(!FileOpen(&file, "rxTools/data/0004013800000102.bin", 0)) return -3;
	FileClose(&file);
	if(!FileOpen(&file, "rxTools/data/data.bin", 0)) return -4;
	FileRead(&file, str, 32, 0);
	FileClose(&file);
	if(memcmp(str, __DATE__, 11)) return -5;
	if(memcmp(&str[12], __TIME__, 8)) return -5;
	return 0;
}

void InstallConfigData(){
	if(CheckInstallationData() == 0)
	{
		first_boot = false;
		return;
	}

	first_boot = true;
	trySetLangFromTheme();
	writeCfg();

	sprintf(str, "/rxTools/Theme/%u/cfg0TOP.bin", cfgs[CFG_THEME].val.i);
	DrawTopSplash(str, str, str);
	sprintf(str, "/rxTools/Theme/%u/cfg0.bin", cfgs[CFG_THEME].val.i);
	DrawBottomSplash(str);

	int res = InstallData("0");	//SD Card
	sprintf(str, "/rxTools/Theme/%u/cfg1%c.bin", cfgs[CFG_THEME].val.i, res == 0 ? 'O' : 'E');
	DrawBottomSplash(str);
	sprintf(str, "/rxTools/Theme/%u/TOP.bin", cfgs[CFG_THEME].val.i);
	sprintf(strl, "/rxTools/Theme/%u/TOPL.bin", cfgs[CFG_THEME].val.i);
	sprintf(strr, "/rxTools/Theme/%u/TOPR.bin", cfgs[CFG_THEME].val.i);
	DrawTopSplash(str, strl, strr);

	InputWait();
}

void trySetLangFromTheme(){
	File MyFile;
	char str[100];
	unsigned int i;

	sprintf(str, "/rxTools/Theme/%u/LANG.txt", cfgs[CFG_THEME].val.i);
	if (!FileOpen(&MyFile, str, 0))
		return;
	if (FileGetSize(&MyFile) > 0)
	{
		FileRead(&MyFile, cfgs[CFG_LANG].val.s, CFG_STR_MAX_LEN, 0);

		for (i = 0; i < CFG_STR_MAX_LEN
			&& cfgs[CFG_LANG].val.s[i] != '\r'
			&& cfgs[CFG_LANG].val.s[i] != '\n'; i++);
		str[i] = 0;

		loadStrings();
	}
	FileClose(&MyFile);
}
