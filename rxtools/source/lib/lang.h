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

#include <wchar.h>
#include "configuration.h"

#define STR_MAX_LEN 64

enum {
	STR_LANG_NAME,
	STR_DUMP,
	STR_DUMPING,
	STR_INJECT,
	STR_CHOOSE,
	STR_NAND,
	STR_SYSNAND,
	STR_EMUNAND,
	STR_BUTTON_ACTION,
	STR_PRESS_BUTTON_ACTION,
	STR_HOLD_BUTTON_ACTION,
	STR_BUTTON_A,
	STR_BUTTON_B,
	STR_BUTTON_X,
	STR_BUTTON_Y,
	STR_BUTTON_L,
	STR_BUTTON_R,
	STR_BUTTON_ZL,
	STR_BUTTON_ZR,
	STR_BUTTON_SELECT,
	STR_BUTTON_HOME,
	STR_BUTTON_START,
	STR_CANCEL,
	STR_CONTINUE,
	STR_FAILED,
	STR_COMPLETED,
	STR_SETTINGS,
	STR_FORCE_UI_BOOT,
	STR_SELECTED_THEME,
	STR_SHOW_AGB,
	STR_ENABLE_3D_UI,
	STR_QUICK_BOOT,
	STR_CONSOLE_LANGUAGE,
	STR_CHOOSE_NAND,
	STR_PRESS_X_SYSNAND,
	STR_PRESS_Y_EMUNAND,
	STR_PRESS_B_BACK,
	STR_MISSING_THEME_FILES,
	STR_AUTOBOOT,
	STR_HOLD_R,
	STR_YES,
	STR_NO,
	STR_NUM
};

extern const char langPath[];
extern wchar_t strings[STR_NUM][STR_MAX_LEN];

int loadStrings(void);
