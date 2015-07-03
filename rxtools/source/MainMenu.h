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
		{ " Decrypt CTR Titles", &CTRDecryptor, "app.bin" },
		{ " Decrypt Title Keys", &DecryptTitleKeys, "app.bin" },
		{ " Decrypt encTitleKeys.bin", &DecryptTitleKeyFile, "app.bin" },
		{ " Generate Xorpads", &PadGen, "app.bin" },
		{ " Decrypt partitions", &DumpNandPartitions, "app.bin" },
		{ " Generate fat16 Xorpad", &GenerateNandXorpads, "app.bin" },
	},
	6,
	0,
	0
};

static Menu DumpMenu = {
	"Dumping Options",
	.Option = (MenuEntry[3]){
		{ " Create NAND dump", &NandDumper, "app.bin" },
		{ " Dump System Titles", &DumpNANDSystemTitles, "app.bin" },
		{ " Dump NAND Files", &dumpCoolFiles, "app.bin" },
	},
	3,
	0,
	0
};

static Menu InjectMenu = {
	"Injection Options",
	.Option = (MenuEntry[2]){
		{ " Inject EmuNAND partitions", &RebuildNand, "app.bin" },
		{ " Inject NAND Files", &restoreCoolFiles, "app.bin" },
	},
	2,
	0,
	0
};

static Menu ExploitMenu = {
	"Other Options",
	.Option = (MenuEntry[3]){
		{ " Downgrade MSET on SysNAND", &downgradeMSET, "app.bin" },
		{ " Install FBI over Health&Safety App", &installFBI, "app.bin" },
		{ " Restore original Health&Safety App", &restoreHS, "app.bin" },
	},
	3,
	0,
	0
};

static Menu SettingsMenu = {
	"Settings",
	.Option = (MenuEntry[2]){
		{ "Force UI boot       ", NULL, "app.bin" },
		{ "Theme selection     ", NULL, "app.bin" },
	},
	2,
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

void ExploitMenuInit(){
	MenuInit(&ExploitMenu);
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
	MenuShow();
	bool autobootgui = false;
	char str[100];

	while (true) {
		u32 pad_state = InputWait();
		if (pad_state & BUTTON_DOWN) MenuNextSelection();
		if (pad_state & BUTTON_UP)   MenuPrevSelection();
		if (pad_state & BUTTON_LEFT || pad_state & BUTTON_RIGHT)
		{
			if (MyMenu->Current == 0) autobootgui = !autobootgui; //autobootgui settings
			else if (MyMenu->Current == 1) //theme selection
			{
				if (pad_state & BUTTON_LEFT && Theme != '0')
				{
					Theme--;
					sprintf(str, "/rxTools/Theme/%c/TOP.bin", Theme);//DRAW TOP SCREEN TO SEE THE NEW THEME
					DrawTopSplash(str);
				}
				else if (pad_state & BUTTON_RIGHT && Theme != '9')
				{
					sprintf(str, "/rxTools/Theme/%c/app.bin", Theme + 1);
					File MyFile;
					if (FileOpen(&MyFile, str, 0))
					{
						FileClose(&MyFile);
						Theme++;
						sprintf(str, "/rxTools/Theme/%c/TOP.bin", Theme);//DRAW TOP SCREEN TO SEE THE NEW THEME
						DrawTopSplash(str);
					}
				}
			}
		}
		if (pad_state & BUTTON_A)
		{
			//Code to save settings
			break;
		}
		if (pad_state & BUTTON_B) 	break;

		TryScreenShot();

		//UPDATE SETTINGS GUI
		if (autobootgui)MyMenu->Option[0].Str = "Force UI boot      <Yes>";
		else MyMenu->Option[0].Str = "Force UI boot      <No >";
		sprintf(str, "Selected Theme:    < %c > ", Theme);
		MyMenu->Option[1].Str = str;
		MenuRefresh();
	}
}

static Menu MainMenu = {
		"rxTools - Roxas75 [v2.6]",
		.Option = (MenuEntry[7]){
			{" Launch rxMode", &rxModeEmu, "menu0.bin"},
			{" Reboot", &rxModeSys, "menu1.bin"},
			{ " Decryption Options", &DecryptMenuInit, "menu2.bin" },
			{ " Dumping Options", &DumpMenuInit, "menu3.bin" },
			{ " Injection Options", &InjectMenuInit, "menu4.bin" },
			{ " Settings", &SettingsMenuInit, "menu5.bin" },
			{ " Credits", NULL, "menu6.bin" },
		},
		7,
		0,
		0
	};

#endif
