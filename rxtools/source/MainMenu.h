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

#ifndef MY_MENU
#define MY_MENU

#include "hid.h"
#include "draw.h"
#include "console.h"
#include "screenshot.h"
#include "menu.h"
#include "fs.h"
#include "CTRDecryptor.h"
#include "NandDumper.h"
#include "TitleKeyDecrypt.h"
#include "padgen.h"
#include "nandtools.h"
#include "downgradeapp.h"
#include "firm.h"
#include "i2c.h"
#include "configuration.h"
#include "lang.h"

static void returnHomeMenu(){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, (unsigned char)(1<<2));
}

static void ShutDown(){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, (uint8_t)(1<<0));
	while(1);
}

static Menu DecryptMenu = {
	L"Decryption Options",
	.Option = (MenuEntry[6]){
		{ L" Decrypt CTR Titles", &CTRDecryptor, "dec0.bin" },
		{ L" Decrypt Title Keys", &DecryptTitleKeys, "dec1.bin" },
		{ L" Decrypt encTitleKeys.bin", &DecryptTitleKeyFile, "dec2.bin" },
		{ L" Generate Xorpads", &PadGen, "dec3.bin" },
		{ L" Decrypt partitions", &DumpNandPartitions, "dec4.bin" },
		{ L" Generate fat16 Xorpad", &GenerateNandXorpads, "dec5.bin" },
	},
	6,
	0,
	0
};

static Menu DumpMenu = {
	L"Dumping Options",
	.Option = (MenuEntry[3]){
		{ L" Create NAND dump", &NandDumper, "dmp0.bin" },
		{ L" Dump System Titles", &DumpNANDSystemTitles, "dmp1.bin" },
		{ L" Dump NAND Files", &dumpCoolFiles, "dmp2.bin" },
	},
	3,
	0,
	0
};

static Menu InjectMenu = {
	L"Injection Options",
	.Option = (MenuEntry[2]){
		{ L" Inject EmuNAND partitions", &RebuildNand, "inj0.bin" },
		{ L" Inject NAND Files", &restoreCoolFiles, "inj1.bin" },
	},
	2,
	0,
	0
};

static Menu AdvancedMenu = {
	L"Other Options",
	.Option = (MenuEntry[5]){
		{ L" Downgrade MSET on SysNAND", &downgradeMSET, "adv0.bin" },
		{ L" Install FBI over Health&Safety App", &installFBI, "adv1.bin" },
		{ L" Restore original Health&Safety App", &restoreHS, "adv2.bin" },
		{ L" Launch DevMode", (void(*)())&DevMode, "adv3.bin" },
		{ L" Load a firm", &FirmLoader, "adv4.bin" },
	},
	5,
	0,
	0
};

static Menu SettingsMenu = {
	L"           SETTINGS",
	.Option = (MenuEntry[7]){
		{ L"Force UI boot               ", NULL, "app.bin" },
		{ L"Selected theme:             ", NULL, "app.bin" },
		{ L"Show AGB_FIRM BIOS:         ", NULL, "app.bin" },
		{ L"Enable 3D UI:               ", NULL, "app.bin" },
		{ L"Quick boot:                 ", NULL, "app.bin" },
		{ L"Autoboot into sysNAND:      ", NULL, "app.bin" },
		{ L"Console language:           ", NULL, "app.bin" },
	},
	7,
	0,
	0
};

void DecryptMenuInit(){
	MenuInit(&DecryptMenu);
	MenuShow();
	while (true) {
		uint32_t pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) MenuNextSelection();
		if(pad_state & BUTTON_UP)   MenuPrevSelection();
		if(pad_state & BUTTON_A)    MenuSelect();
		if(pad_state & BUTTON_B) 	{ MenuClose(); break; }
		TryScreenShot();
		MenuShow();
	}
}

void DumpMenuInit(){
	MenuInit(&DumpMenu);
	MenuShow();
	while (true) {
		uint32_t pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) MenuNextSelection();
		if(pad_state & BUTTON_UP)   MenuPrevSelection();
		if(pad_state & BUTTON_A)    MenuSelect();
		if(pad_state & BUTTON_B) 	{ MenuClose(); break; }
		TryScreenShot();
		MenuShow();
	}
}

void InjectMenuInit(){
	MenuInit(&InjectMenu);
	MenuShow();
	while (true) {
		uint32_t pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) MenuNextSelection();
		if(pad_state & BUTTON_UP)   MenuPrevSelection();
		if(pad_state & BUTTON_A)    MenuSelect();
		if(pad_state & BUTTON_B) 	{ MenuClose(); break; }
		TryScreenShot();
		MenuShow();
	}
}

void AdvancedMenuInit(){
	MenuInit(&AdvancedMenu);
	MenuShow();
	while (true) {
		uint32_t pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) MenuNextSelection();
		if(pad_state & BUTTON_UP)   MenuPrevSelection();
		if(pad_state & BUTTON_A)    MenuSelect();
		if(pad_state & BUTTON_B) 	{ MenuClose(); break; }
		TryScreenShot();
		MenuShow();
	}
}

void SettingsMenuInit(){
	MenuInit(&SettingsMenu);
	DIR d;
	FILINFO fno;
	char str[100];
	char strl[100];
	char strr[100];
	const unsigned int maxLangNum = 16;
	char langs[maxLangNum][CFG_STR_MAX_LEN];
	unsigned char theme_num = 0;
	unsigned int curLang, langNum;

	curLang = 0;
	langNum = 0;

	if (!f_opendir(&d, langPath)) {
		while (langNum < maxLangNum) {
			fno.lfname = langs[langNum];
			fno.lfsize = CFG_STR_MAX_LEN;

			if (f_readdir(&d, &fno))
				break;

			if (fno.fname[0] == 0)
				break;

			if (langs[langNum][0] == 0)
				strcpy(langs[langNum], fno.fname);
			else if (!strcmp(langs[langNum], cfgs[CFG_LANG].val.s))
				curLang = langNum;

			if (!strcmp(fno.fname, cfgs[CFG_LANG].val.s))
				curLang = langNum;

			langNum++;
		}

		f_closedir(&d);
	}

	while (true) {
		//UPDATE SETTINGS GUI
		swprintf(MyMenu->Name, CONSOLE_MAX_TITLE_LENGTH+1, strings[STR_SETTINGS]);
		swprintf(MyMenu->Option[0].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_FORCE_UI_BOOT], cfgs[CFG_GUI].val.i ? strings[STR_ENABLED] : strings[STR_DISABLED]);
		swprintf(MyMenu->Option[1].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_SELECTED_THEME], theme_num + '0');
		swprintf(MyMenu->Option[2].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_SHOW_AGB], cfgs[CFG_AGB].val.i ? strings[STR_ENABLED] : strings[STR_DISABLED]);
		swprintf(MyMenu->Option[3].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_ENABLE_3D_UI], cfgs[CFG_3D].val.i ? strings[STR_ENABLED] : strings[STR_DISABLED]);
		swprintf(MyMenu->Option[4].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_QUICK_BOOT], cfgs[CFG_SILENT].val.i ? strings[STR_ENABLED] : strings[STR_DISABLED]);
		swprintf(MyMenu->Option[5].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_ABSYSN], cfgs[CFG_ABSYSN].val.i ? strings[STR_ENABLED] : strings[STR_DISABLED]);
		swprintf(MyMenu->Option[6].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_MENU_LANGUAGE], strings[STR_LANG_NAME]);
		MenuRefresh();

		uint32_t pad_state = InputWait();
		if (pad_state & BUTTON_DOWN) MenuNextSelection();
		if (pad_state & BUTTON_UP)   MenuPrevSelection();
		if (pad_state & BUTTON_LEFT || pad_state & BUTTON_RIGHT)
		{
			if (MyMenu->Current == 0)
			{
				cfgs[CFG_GUI].val.i ^= 1;
				if (cfgs[CFG_GUI].val.i)
					cfgs[CFG_SILENT].val.i = 0;
			}
			else if (MyMenu->Current == 1) //theme selection
			{
				/* Jump to the next available theme */
				File AppBin;
				bool found = false;
				unsigned char i;

				if (pad_state & BUTTON_LEFT && theme_num > 0)
				{
					for (i = theme_num - 1; i > 0; i--)
					{
						sprintf(str, "/rxTools/Theme/%u/app.bin", i);
						if (FileOpen(&AppBin, str, 0))
						{
							FileClose(&AppBin);
							found = true;
							break;
						}
					}

					if (i == 0) found = true;
				} else
				if (pad_state & BUTTON_RIGHT && theme_num < 9)
				{
					for (i = theme_num + 1; i <= 9; i++)
					{
						sprintf(str, "/rxTools/Theme/%u/app.bin", i);
						if (FileOpen(&AppBin, str, 0))
						{
							FileClose(&AppBin);
							found = true;
							break;
						}
					}
				}

				if (found)
				{
					theme_num = i;
					sprintf(str, "/rxTools/Theme/%u/TOP.bin", theme_num);
					if (cfgs[CFG_3D].val.i)
					{
						sprintf(strl, "/rxTools/Theme/%u/TOPL.bin", theme_num);
						sprintf(strr, "/rxTools/Theme/%u/TOPR.bin", theme_num);
						DrawTopSplash(str, strl, strr);
					}
					else
					{
						sprintf(str, "/rxTools/Theme/%u/TOP.bin", theme_num);
						DrawTopSplash(str, str, str);
					}

					cfgs[CFG_THEME].val.i = theme_num;
					trySetLangFromTheme(1);
				}
			}
			else if (MyMenu->Current == 2) cfgs[CFG_AGB].val.i ^= 1;
			else if (MyMenu->Current == 3)
			{
				cfgs[CFG_3D].val.i ^= 1;
				sprintf(str, "/rxTools/Theme/%u/TOP.bin", theme_num);
				if(cfgs[CFG_3D].val.i)
				{
					sprintf(strl, "/rxTools/Theme/%u/TOPL.bin", theme_num);
					sprintf(strr, "/rxTools/Theme/%u/TOPR.bin", theme_num);
					DrawTopSplash(str, strl, strr);
				}
				else
				{
					sprintf(str, "/rxTools/Theme/%u/TOP.bin", theme_num);
					DrawTopSplash(str, str, str);
				}
			}
			else if (MyMenu->Current == 4)
			{
				cfgs[CFG_SILENT].val.i ^= 1;
				if (cfgs[CFG_SILENT].val.i)
					cfgs[CFG_GUI].val.i = 0;
			}
			else if (MyMenu->Current == 5)
			{
				cfgs[CFG_ABSYSN].val.i ^= 1;
			}
			else if (MyMenu->Current == 6)
			{
				if (pad_state & BUTTON_LEFT && curLang > 0)
					curLang--;
				else if (pad_state & BUTTON_RIGHT && curLang + 1 < langNum)
					curLang++;

				strcpy(cfgs[CFG_LANG].val.s, langs[curLang]);
				preloadStringsOnSwitch();
				loadStrings();
			}
		}
		if (pad_state & BUTTON_B)
		{
			//Code to save settings
			writeCfg();
			MenuClose();
			break;
		}

		TryScreenShot();

	}
}

void BootMenuInit(){
	char str[100];
	sprintf(str, "/rxTools/Theme/%u/boot0.bin", cfgs[CFG_THEME].val.i);//DRAW TOP SCREEN TO SEE THE NEW THEME
	DrawBottomSplash(str);
	while (true) {
		uint32_t pad_state = InputWait();
		if (pad_state & BUTTON_Y) {
			rxModeWithSplash(1);      //Boot emunand
			DrawBottomSplash(str);
		} else if (pad_state & BUTTON_X) {
			rxModeWithSplash(0); //Boot sysnand
			DrawBottomSplash(str);
		} else if (pad_state & BUTTON_B)
			break;
	}

	MenuClose();
}

void CreditsMenuInit(){
	char str[100];
	sprintf(str, "/rxTools/Theme/%u/credits.bin", cfgs[CFG_THEME].val.i);//DRAW TOP SCREEN TO SEE THE NEW THEME
	DrawBottomSplash(str);
	WaitForButton(BUTTON_B);
	OpenAnimation();
}

static Menu MainMenu = {
		L"rxTools - Roxas75 [v3.0]",
		.Option = (MenuEntry[7]){
			{ L" Launch rxMode", &BootMenuInit, "menu0.bin" },
			{ L" Decryption Options", &DecryptMenuInit, "menu1.bin" },
			{ L" Dumping Options", &DumpMenuInit, "menu2.bin" },
			{ L" Injection Options", &InjectMenuInit, "menu3.bin" },
			{ L" Advanced Options", &AdvancedMenuInit, "menu4.bin" },
			{ L" Settings", &SettingsMenuInit, "menu5.bin" },
			{ L" Credits", &CreditsMenuInit, "menu6.bin" },
		},
		7,
		0,
		0
	};

#endif
