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
#include "cfw.h"
#include "i2c.h"
#include "configuration.h"
#include "lang.h"

#define ENABLED		L"✔"
#define DISABLED	L"✘"
//#define ENABLED		L"⦿"
//#define DISABLED	L"⦾"

static void returnHomeMenu(){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, (unsigned char)(1<<2));
}

static void ShutDown(){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, (u8)(1<<0));
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
	.Option = (MenuEntry[4]){
		{ L" Downgrade MSET on SysNAND", &downgradeMSET, "adv0.bin" },
		{ L" Install FBI over Health&Safety App", &installFBI, "adv1.bin" },
		{ L" Restore original Health&Safety App", &restoreHS, "adv2.bin" },
		{ L" Launch DevMode", &DevMode, "adv3.bin" },
	},
	4,
	0,
	0
};

static Menu SettingsMenu = {
	L"           SETTINGS",
	.Option = (MenuEntry[6]){
		{ L"Force UI boot               ", NULL, "app.bin" },
		{ L"Selected theme:             ", NULL, "app.bin" },
		{ L"Show AGB_FIRM BIOS:         ", NULL, "app.bin" },
		{ L"Enable 3D UI:               ", NULL, "app.bin" },
		{ L"Quick boot:                 ", NULL, "app.bin" },
		{ L"Console language:           ", NULL, "app.bin" },
	},
	6,
	0,
	0
};

void DecryptMenuInit(){
	MenuInit(&DecryptMenu);
	MenuShow();
    while (true) {
        u32 pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) MenuNextSelection();
		if(pad_state & BUTTON_UP)   MenuPrevSelection();
		if(pad_state & BUTTON_A)    MenuSelect();
		if(pad_state & BUTTON_B) 	break;
		TryScreenShot();
		MenuShow();
    }
}

void DumpMenuInit(){
	MenuInit(&DumpMenu);
	MenuShow();
    while (true) {
        u32 pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) MenuNextSelection();
		if(pad_state & BUTTON_UP)   MenuPrevSelection();
		if(pad_state & BUTTON_A)    MenuSelect();
		if(pad_state & BUTTON_B) 	break;
		TryScreenShot();
		MenuShow();
    }
}

void InjectMenuInit(){
	MenuInit(&InjectMenu);
	MenuShow();
    while (true) {
        u32 pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) MenuNextSelection();
		if(pad_state & BUTTON_UP)   MenuPrevSelection();
		if(pad_state & BUTTON_A)    MenuSelect();
		if(pad_state & BUTTON_B) 	break;
		TryScreenShot();
		MenuShow();
    }
}

void AdvancedMenuInit(){
	MenuInit(&AdvancedMenu);
	MenuShow();
    while (true) {
        u32 pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) MenuNextSelection();
		if(pad_state & BUTTON_UP)   MenuPrevSelection();
		if(pad_state & BUTTON_A)    MenuSelect();
		if(pad_state & BUTTON_B) 	break;
		TryScreenShot();
		MenuShow();
    }
}

void SettingsMenuInit(){
	MenuInit(&SettingsMenu);
	char str[100];
	char strl[100];
	char strr[100];
	char settings[] = "000100";
	unsigned char theme_num = 0;

	File MyFile;
	if (FileOpen(&MyFile, "/rxTools/data/system.txt", 0))
	{
		FileRead(&MyFile, settings, 6, 0);
		bootGUI = (settings[0] == '1');
		theme_num = (settings[1] - '0');
		agb_bios = (settings[2] == '1');
		theme_3d = (settings[3] == '1');
		silent_boot = (settings[4] == '1');
		language = (settings[5] - '0');
	}

	while (true) {
		u32 pad_state = InputWait();
		if (pad_state & BUTTON_DOWN) MenuNextSelection();
		if (pad_state & BUTTON_UP)   MenuPrevSelection();
		if (pad_state & BUTTON_LEFT || pad_state & BUTTON_RIGHT)
		{
			if (MyMenu->Current == 0)
			{
				bootGUI ^= 1; //bootGUI settings
				if(bootGUI)
					silent_boot=0;
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
					if(theme_3d)
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

					Theme = (theme_num + '0');
					File MyFile;
					sprintf(str, "/rxTools/Theme/%u/LANG.txt", theme_num);
					if (FileOpen(&MyFile, str, 0))
					{
						if (FileGetSize(&MyFile) > 0)
						{
							char tl[]="00";
							FileRead(&MyFile, tl, 1, 0);
							if(tl[0] - '0' >= 0 && tl[0] - '0' <= N_LANG)
								language = tl[0] - '0';
						}
					}
				}
			}
			else if (MyMenu->Current == 2) agb_bios ^= 1; //AGB_FIRM BIOS
			else if (MyMenu->Current == 3)
			{
				theme_3d ^= 1; //3D UI
				sprintf(str, "/rxTools/Theme/%u/TOP.bin", theme_num);
				if(theme_3d)
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
				silent_boot ^= 1; //SILENT BOOT
				if(silent_boot)
					bootGUI = 0;
			}
			else if (MyMenu->Current == 5) //language selection
			{
				if (pad_state & BUTTON_LEFT && language > 0)
				{
					language--;
				} else

				if (pad_state & BUTTON_RIGHT && language < N_LANG)
				{
					language++;
				}
			}
		}
		if (pad_state & BUTTON_B)
		{
			//Code to save settings
			Theme = (theme_num + '0');
			settings[0] = bootGUI ? '1' : '0';
			settings[1] = Theme;
			settings[2] = agb_bios ? '1' : '0';
			settings[3] = theme_3d ? '1' : '0';
			settings[4] = silent_boot ? '1' : '0';
			settings[5] = language + '0';
			FileWrite(&MyFile, settings, 6, 0);
			FileClose(&MyFile);
			break;
		}

		TryScreenShot();

		//UPDATE SETTINGS GUI
		swprintf(MyMenu->Option[0].Str, CONSOLE_MAX_LINE_LENGTH+1, STR_FORCE_UI_BOOT[language], bootGUI ? ENABLED : DISABLED);
		swprintf(MyMenu->Option[1].Str, CONSOLE_MAX_LINE_LENGTH+1, STR_SELECTED_THEME[language], theme_num + '0');
		swprintf(MyMenu->Option[2].Str, CONSOLE_MAX_LINE_LENGTH+1, STR_SHOW_AGB[language], agb_bios ? ENABLED : DISABLED);
		swprintf(MyMenu->Option[3].Str, CONSOLE_MAX_LINE_LENGTH+1, STR_ENABLE_3D_UI[language], theme_3d ? ENABLED : DISABLED);
		swprintf(MyMenu->Option[4].Str, CONSOLE_MAX_LINE_LENGTH+1, STR_QUICK_BOOT[language], silent_boot ? ENABLED : DISABLED);
		swprintf(MyMenu->Option[5].Str, CONSOLE_MAX_LINE_LENGTH+1, STR_CONSOLE_LANGUAGE[language], STR_LANGUAGES[language]);
		MenuRefresh();
	}
}

void BootMenuInit(){
	char str[100];
	sprintf(str, "/rxTools/Theme/%c/boot0.bin", Theme);//DRAW TOP SCREEN TO SEE THE NEW THEME
	DrawBottomSplash(str);
	while (true) {
		u32 pad_state = InputWait();
		if (pad_state & BUTTON_Y) rxModeEmu();      //Boot emunand
		else if (pad_state & BUTTON_X) rxModeSys(); //Boot sysnand
		else if (pad_state & BUTTON_B) break; //Boot sysnand
	}
}

void CreditsMenuInit(){
	char str[100];
	sprintf(str, "/rxTools/Theme/%c/credits.bin", Theme);//DRAW TOP SCREEN TO SEE THE NEW THEME
	DrawBottomSplash(str);
	WaitForButton(BUTTON_B);
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
