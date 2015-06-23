#ifndef MY_MENU
#define MY_MENU

#include "hid.h"
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

static void returnHomeMenu(){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, (unsigned char)(1<<2));
}

static void ShutDown(){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, (u8)(1<<0));
	while(1);
}

static Menu DecryptMenu = {
	"Decryption Options",
	{
		" Decrypt CTR Titles", &CTRDecryptor,
		" Decrypt Title Keys", &DecryptTitleKeys,
		" Generate Xorpads", &PadGen,
		" Decrypt partitions", &DumpNandPartitions,
		" Generate fat16 Xorpad", &GenerateNandXorpads,
	},
	5,
	0,
	0
};

static Menu DumpMenu = {
	"Dumping Options",
	{
		" Create NAND dump", &NandDumper,
		" Dump System Titles", &DumpNANDSystemTitles,
		" Dump NAND Files", &dumpCoolFiles,
	},
	3,
	0,
	0
};

static Menu InjectMenu = {
	"Injection Options",
	{
		" Inject EmuNAND partitions", &RebuildNand,
		" Inject NAND Files", &restoreCoolFiles,
	},
	2,
	0,
	0
};

static Menu ExploitMenu = {
	"Other Options",
	{
		" Downgrade MSET on SysNAND", &downgradeMSET,
		" Install FBI over Health&Safety App", &installFBI,
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

static void Credits(){
	ConsoleInit();
	ConsoleSetTitle("Credits");
	print("Roxas75 - Overall Developer\n");
	print("3dbrew - Best documentation recipe\n");
	print("GbaTemp users - For help/testing\n");
	print("Gateway Team - Real researchers\n");
	print("Archshift - Author of useful libs\n");
	print("patois - BRAHMA Author\n");
	print("\nPress B to exit\n"); ConsoleShow();
	WaitForButton(BUTTON_B);
}

static Menu MainMenu = {
		"rxTools - Roxas75 [v2.5]",
		{
			" Launch rxMode", &LaunchCfw,
			" Decryption Options", &DecryptMenuInit,
			" Dumping Options", &DumpMenuInit,
			" Injection Options", &InjectMenuInit,
			" Other Options", &ExploitMenuInit,
			" Credits", &Credits,
			" DevMode", &DevMode,
		},
		6,
		0,
		0
	};

#endif
