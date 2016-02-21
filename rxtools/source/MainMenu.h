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

#include <lib/hid.h>
#include <lib/ui/draw.h>
#include <lib/ui/console.h>
#include <features/screenshot.h>
#include <lib/menu.h>
#include <lib/fs.h>
#include <features/AdvancedFileManager.h>
#include <features/CTRDecryptor.h>
#include <features/NandDumper.h>
#include <features/TitleKeyDecrypt.h>
#include <features/padgen.h>
#include <features/nandtools.h>
#include <features/downgradeapp.h>
#include <features/firm.h>
#include <lib/i2c.h>
#include <lib/cfg.h>
#include <lib/lang.h>

static void ShutDown(int arg){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, (arg) ? (uint8_t)(1<<0):(uint8_t)(1<<2));
	while(1);
}

static Menu DecryptMenu = {
	L"Decryption Options",
	.Option = (MenuEntry[6]){
		{ L" Decrypt CTR Titles", &CTRDecryptor, L"dec0.bin" },
		{ L" Decrypt Title Keys", &DecryptTitleKeys, L"dec1.bin" },
		{ L" Decrypt encTitleKeys.bin", &DecryptTitleKeyFile, L"dec2.bin" },
		{ L" Generate Xorpads", &PadGen, L"dec3.bin" },
		{ L" Decrypt partitions", &DumpNandPartitions, L"dec4.bin" },
		{ L" Generate fat16 Xorpad", &GenerateNandXorpads, L"dec5.bin" },
	},
	6,
	0,
	0
};

static Menu DumpMenu = {
	L"Dumping Options",
	.Option = (MenuEntry[3]){
		{ L" Create NAND dump", &NandDumper, L"dmp0.bin" },
		{ L" Dump System Titles", &DumpNANDSystemTitles, L"dmp1.bin" },
		{ L" Dump NAND Files", &dumpCoolFiles, L"dmp2.bin" },
	},
	3,
	0,
	0
};

static Menu InjectMenu = {
	L"Injection Options",
	.Option = (MenuEntry[2]){
		{ L" Inject EmuNAND partitions", &RebuildNand, L"inj0.bin" },
		{ L" Inject NAND Files", &restoreCoolFiles, L"inj1.bin" },
	},
	2,
	0,
	0
};

static Menu AdvancedMenu = {
	L"Other Options",
	.Option = (MenuEntry[5]){
		{ L" Downgrade MSET on SysNAND", &downgradeMSET, L"adv0.bin" },
		{ L" Install FBI over H&S App", &installFBI, L"adv1.bin" },
		{ L" Restore original H&S App", &restoreHS, L"adv2.bin" },
		{ L" Launch PastaMode", (void(*)())&PastaMode, L"adv3.bin" },
		{ L" Advanced File Manager", &AdvFileManagerMain, L"adv4.bin" },
	},
	5,
	0,
	0
};

static Menu SettingsMenu = {
	L"           SETTINGS",
	.Option = (MenuEntry[9]){
		{ L"Force UI boot               ", NULL, L"app.bin" },
		{ L"Selected theme:             ", NULL, L"app.bin" },
		{ L"Random theme:               ", NULL, L"app.bin" },
		{ L"Show AGB_FIRM BIOS:         ", NULL, L"app.bin" },
		{ L"Enable 3D UI:               ", NULL, L"app.bin" },
		{ L"Autoboot into sysNAND:      ", NULL, L"app.bin" },
		{ L"Console language:           ", NULL, L"app.bin" },
		{ L"Reboot                      ", NULL, L"app.bin" },
		{ L"Shutdown                    ", NULL, L"app.bin" },
	},
	9,
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
	wchar_t str[_MAX_LFN];
	wchar_t strl[_MAX_LFN];
	wchar_t strr[_MAX_LFN];
	const unsigned int maxLangNum = 16;
	TCHAR langs[maxLangNum][CFG_STR_MAX_LEN];
	unsigned char theme_num = 0;
	unsigned int curLang, langNum;

	curLang = 0;
	langNum = 0;

	if (!f_opendir(&d, langPath)) {
		mbstowcs(str, cfgs[CFG_LANG].val.s, _MAX_LFN);

		while (langNum < maxLangNum) {
			fno.lfname = langs[langNum];
			fno.lfsize = CFG_STR_MAX_LEN;

			if (f_readdir(&d, &fno))
				break;

			if (fno.lfname[0] == 0)
				break;

			if (!wcscmp(fno.lfname, str))
				curLang = langNum;

			langNum++;
		}

		f_closedir(&d);
	}

	while (true) {
		//UPDATE SETTINGS GUI
		swprintf(MyMenu->Name, CONSOLE_MAX_TITLE_LENGTH+1, L"%ls    Build: %s", strings[STR_SETTINGS], VERSION);
		swprintf(MyMenu->Option[0].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_FORCE_UI_BOOT], cfgs[CFG_GUI].val.i ? strings[STR_ENABLED] : strings[STR_DISABLED]);
		swprintf(MyMenu->Option[1].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_SELECTED_THEME], cfgs[CFG_THEME].val.i + '0');
		swprintf(MyMenu->Option[2].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_RANDOM], cfgs[CFG_RANDOM].val.i ? strings[STR_ENABLED] : strings[STR_DISABLED]);
		swprintf(MyMenu->Option[3].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_SHOW_AGB], cfgs[CFG_AGB].val.i ? strings[STR_ENABLED] : strings[STR_DISABLED]);
		swprintf(MyMenu->Option[4].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_ENABLE_3D_UI], cfgs[CFG_3D].val.i ? strings[STR_ENABLED] : strings[STR_DISABLED]);
		swprintf(MyMenu->Option[5].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_ABSYSN], cfgs[CFG_ABSYSN].val.i ? strings[STR_ENABLED] : strings[STR_DISABLED]);
		swprintf(MyMenu->Option[6].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_MENU_LANGUAGE], strings[STR_LANG_NAME]);
		swprintf(MyMenu->Option[7].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_SHUTDOWN]);
		swprintf(MyMenu->Option[8].Str, CONSOLE_MAX_LINE_LENGTH+1, strings[STR_REBOOT]);
		MenuRefresh();

		uint32_t pad_state = InputWait();
		if (pad_state & BUTTON_DOWN)
		{
			if(MyMenu->Current == 7) MenuNextSelection();
			MenuNextSelection();
		}
		if (pad_state & BUTTON_UP)
		{
			if(MyMenu->Current == 0) MenuPrevSelection();
			MenuPrevSelection();
		}
		if (pad_state & BUTTON_LEFT || pad_state & BUTTON_RIGHT)
		{
			if (MyMenu->Current == 0)
			{
				cfgs[CFG_GUI].val.i ^= 1;
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
						swprintf(str, _MAX_LFN, L"/rxTools/Theme/%u/app.bin", i);
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
						swprintf(str, _MAX_LFN, L"/rxTools/Theme/%u/app.bin", i);
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
					swprintf(str, _MAX_LFN, L"/rxTools/Theme/%u/TOP.bin", theme_num);
					if (cfgs[CFG_3D].val.i)
					{
						swprintf(strl, _MAX_LFN, L"/rxTools/Theme/%u/TOPL.bin", theme_num);
						swprintf(strr, _MAX_LFN, L"/rxTools/Theme/%u/TOPR.bin", theme_num);
						DrawTopSplash(str, strl, strr);
					}
					else
					{
						swprintf(str, _MAX_LFN, L"/rxTools/Theme/%u/TOP.bin", theme_num);
						DrawTopSplash(str, str, str);
					}

					cfgs[CFG_THEME].val.i = theme_num;
					trySetLangFromTheme(1);
				}
			}
			else if (MyMenu->Current == 2) cfgs[CFG_RANDOM].val.i ^= 1;
			else if (MyMenu->Current == 3) cfgs[CFG_AGB].val.i ^= 1;
			else if (MyMenu->Current == 4)
			{
				cfgs[CFG_3D].val.i ^= 1;
				swprintf(str, _MAX_LFN, L"/rxTools/Theme/%u/TOP.bin", theme_num);
				if(cfgs[CFG_3D].val.i)
				{
					swprintf(strl, _MAX_LFN, L"/rxTools/Theme/%u/TOPL.bin", theme_num);
					swprintf(strr, _MAX_LFN, L"/rxTools/Theme/%u/TOPR.bin", theme_num);
					DrawTopSplash(str, strl, strr);
				}
				else
				{
					swprintf(str, _MAX_LFN, L"/rxTools/Theme/%u/TOP.bin", theme_num);
					DrawTopSplash(str, str, str);
				}
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

				wcstombs(cfgs[CFG_LANG].val.s, langs[curLang],
					CFG_STR_MAX_LEN);
				switchStrings();
			}
			else if (MyMenu->Current == 7)
			{
				MenuNextSelection();
			}
			else if (MyMenu->Current == 8)
			{
				MenuPrevSelection();
			}
		}
		else if (pad_state & BUTTON_A)
		{
			if (MyMenu->Current == 7)
			{
				fadeOut();
				ShutDown(1);
			}
			else if (MyMenu->Current == 8)
			{
				fadeOut();
				ShutDown(0);
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
	wchar_t str[_MAX_LFN];

	//SHOW ONLY SYSYNAND IF EMUNAND IS NOT FOUND
	swprintf(str, _MAX_LFN, L"/rxTools/Theme/%u/boot%c.bin",
		cfgs[CFG_THEME].val.i, checkEmuNAND() ? L'0' : L'S');
	DrawBottomSplash(str);
	while (true) {
		uint32_t pad_state = InputWait();
		if ((pad_state & BUTTON_Y) && checkEmuNAND()) {
			rxModeWithSplash(1);      //Boot emunand (only if found)
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
	wchar_t str[_MAX_LFN];
	swprintf(str, _MAX_LFN, L"/rxTools/Theme/%u/credits.bin",
		cfgs[CFG_THEME].val.i);
	DrawBottomSplash(str);
	WaitForButton(BUTTON_B);
	OpenAnimation();
}

static Menu MainMenu = {
		L"rxTools - Roxas75 [v3.0]",
		.Option = (MenuEntry[7]){
			{ L" Launch rxMode", &BootMenuInit, L"menu0.bin" },
			{ L" Decryption Options", &DecryptMenuInit, L"menu1.bin" },
			{ L" Dumping Options", &DumpMenuInit, L"menu2.bin" },
			{ L" Injection Options", &InjectMenuInit, L"menu3.bin" },
			{ L" Advanced Options", &AdvancedMenuInit, L"menu4.bin" },
			{ L" Settings", &SettingsMenuInit, L"menu5.bin" },
			{ L" Credits", &CreditsMenuInit, L"menu6.bin" },
		},
		7,
		0,
		0
	};

#endif
