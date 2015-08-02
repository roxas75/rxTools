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

#include "configuration.h"

#define LANG_CODE_LEN 5

#if LANG_CODE_NUM > CFG_STR_MAX_LEN
#error "LANG_CODE_LEN > CFG_STR_MAX_LEN"
#endif

enum {
	STR_LANG_EN,
	STR_LANG_IT,
	STR_LANG_ES,
	STR_LANG_FR,
	STR_LANG_NL,
	STR_LANG_NO,
	STR_LANG_HR,
	STR_LANG_RU,
	STR_LANG_ZH_CN,
	STR_LANG_ZH_TW,
	STR_LANG_PT,

	STR_LANG_NUM
};

enum {
	STR_LANG_NAME,

	STR_FORCE_UI_BOOT,
	STR_SELECTED_THEME,
	STR_SHOW_AGB,
	STR_ENABLE_3D_UI,
	STR_QUICK_BOOT,
	STR_CONSOLE_LANGUAGE,
	STR_AUTOBOOT,
	STR_HOLD_R,

	STR_NUM
};

extern const wchar_t * const *strings;

void setLang(unsigned int i);
void setLangByCode(const char *code);
unsigned int getLang(void);
const char *getLangCode(void);
