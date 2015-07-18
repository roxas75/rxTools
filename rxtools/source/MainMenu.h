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

static void returnHomeMenu(){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, (unsigned char)(1<<2));
}

static void ShutDown(){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, (u8)(1<<0));
	while(1);
}

static Menu DecryptMenu = {
	"Decryption Options",
	.Option = (MenuEntry[6]){
		{ " Decrypt CTR Titles", &CTRDecryptor, "dec0.bin" },
		{ " Decrypt Title Keys", &DecryptTitleKeys, "dec1.bin" },
		{ " Decrypt encTitleKeys.bin", &DecryptTitleKeyFile, "dec2.bin" },
		{ " Generate Xorpads", &PadGen, "dec3.bin" },
		{ " Decrypt partitions", &DumpNandPartitions, "dec4.bin" },
		{ " Generate fat16 Xorpad", &GenerateNandXorpads, "dec5.bin" },
	},
	6,
	0,
	0
};

static Menu DumpMenu = {
	"Dumping Options",
	.Option = (MenuEntry[3]){
		{ " Create NAND dump", &NandDumper, "dmp0.bin" },
		{ " Dump System Titles", &DumpNANDSystemTitles, "dmp1.bin" },
		{ " Dump NAND Files", &dumpCoolFiles, "dmp2.bin" },
	},
	3,
	0,
	0
};

static Menu InjectMenu = {
	"Injection Options",
	.Option = (MenuEntry[2]){
		{ " Inject EmuNAND partitions", &RebuildNand, "inj0.bin" },
		{ " Inject NAND Files", &restoreCoolFiles, "inj1.bin" },
	},
	2,
	0,
	0
};

static Menu AdvancedMenu = {
	"Other Options",
	.Option = (MenuEntry[4]){
		{ " Downgrade MSET on SysNAND", &downgradeMSET, "adv0.bin" },
		{ " Install FBI over Health&Safety App", &installFBI, "adv1.bin" },
		{ " Restore original Health&Safety App", &restoreHS, "adv2.bin" },
		{ " Launch DevMode", &DevMode, "adv3.bin" },
	},
	4,
	0,
	0
};

static Menu SettingsMenu = {
	"           SETTINGS",
	.Option = (MenuEntry[5]){
		{ "Force UI boot               ", NULL, "app.bin" },
		{ "Selected Theme:             ", NULL, "app.bin" },
		{ "Show AGB_FIRM BIOS:         ", NULL, "app.bin" },
		{ "Enable 3D UI:               ", NULL, "app.bin" },
		{ "Silent/quick boot:          ", NULL, "app.bin" },
	},
	5,
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
	char settings[] = "00010";
	unsigned char theme_num = 0;
	
	File MyFile;
	if (FileOpen(&MyFile, "/rxTools/data/system.txt", 0))
	{
		FileRead(&MyFile, settings, 5, 0);
		bootGUI = (settings[0] == '1');
		theme_num = (settings[1] - 0x30);
		agb_bios = (settings[2] == '1');
		theme_3d = (settings[3] == '1');
		silent_boot = (settings[4] == '1');
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

					Theme = (theme_num + 0x30);
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
		}
		if (pad_state & BUTTON_B)
		{
			//Code to save settings
			Theme = (theme_num + 0x30);
			settings[0] = bootGUI ? '1' : '0';
			settings[1] = Theme;
			settings[2] = agb_bios ? '1' : '0';
			settings[3] = theme_3d ? '1' : '0';
			settings[4] = silent_boot ? '1' : '0';
			FileWrite(&MyFile, settings, 5, 0);
			FileClose(&MyFile);
			break;
		}
		
		TryScreenShot();
		
		//UPDATE SETTINGS GUI
		sprintf(MyMenu->Option[0].Str, "Force UI boot:      < %s > ", bootGUI ? "Yes" : "No ");
		sprintf(MyMenu->Option[1].Str, "Selected Theme:     <  %c  > ", theme_num + 0x30);
		sprintf(MyMenu->Option[2].Str, "Show AGB_FIRM BIOS: < %s > ", agb_bios ? "Yes" : "No ");
		sprintf(MyMenu->Option[3].Str, "Enable 3D UI:       < %s > ", theme_3d ? "Yes" : "No ");
		sprintf(MyMenu->Option[4].Str, "Silent/quick boot:  < %s > ", silent_boot ? "Yes" : "No ");
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
		"rxTools - Roxas75 [v3.0]",
		.Option = (MenuEntry[7]){
			{ " Launch rxMode", &BootMenuInit, "menu0.bin" },
			{ " Decryption Options", &DecryptMenuInit, "menu1.bin" },
			{ " Dumping Options", &DumpMenuInit, "menu2.bin" },
			{ " Injection Options", &InjectMenuInit, "menu3.bin" },
			{ " Advanced Options", &AdvancedMenuInit, "menu4.bin" },
			{ " Settings", &SettingsMenuInit, "menu5.bin" },
			{ " Credits", &CreditsMenuInit, "menu6.bin" },
		},
		7,
		0,
		0
	};

#endif
