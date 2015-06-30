#include "configuration.h"
#include "common.h"
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

#define DATAFOLDER "rxtools/data"
#define KEYFILENAME "slot0x25KeyX.bin"
#define WORKBUF (u8*)0x21000000
#define NAT_SIZE 0xEBC00
#define AGB_SIZE 0xD9C00
#define TWL_SIZE 0x1A1C00

char tmpstr[256] = {0};
File tempfile;
UINT tmpu32;

int InstallData(char* drive){
	FIL firmfile;
	char* progressbar = "[       ]";
	char* progress = progressbar+1;
	print("%s", progressbar); ConsoleShow(); ConsolePrevLine();
	
	//Create the workdir
	sprintf(tmpstr, "%s:%s", drive, DATAFOLDER);
	f_mkdir(tmpstr);
	f_chmod(tmpstr, AM_HID, AM_HID);
	
	//Read firmware data
	if(f_open(&firmfile, "firmware.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
		//... We'll see
	}else return CONF_NOFIRMBIN;
	*progress++ = '.'; print("%s", progressbar); ConsoleShow(); ConsolePrevLine();
	
	//Create patched native_firm
	f_read(&firmfile, WORKBUF, NAT_SIZE, &tmpu32);
	u8* n_firm = decryptFirmTitle(WORKBUF, NAT_SIZE, 0x00000002);
	u8* n_firm_patch = GetFilePack("nat_patch.bin");
	applyPatch(n_firm, n_firm_patch);
	u8 keyx[16] = {0};
	if(GetSystemVersion() < 3){
		FileOpen(&tempfile, KEYFILENAME, 0);
		FileRead(&tempfile, &keyx[0], 16, 0);
		FileClose(&tempfile);
	}
	*progress++ = '.'; print("%s", progressbar); ConsoleShow(); ConsolePrevLine();
	for(int i = 0; i < NAT_SIZE; i+=0x4){
		if(!strcmp((char*)n_firm + i, "Shit")){
			if(1){
				memcpy((char*)n_firm + i, rxmode_emu_label, 4);
			}else{
				memcpy((char*)n_firm + i, rxmode_sys_label , 4);
			}
		}
		if(!strcmp((char*)n_firm + i, "InsertKeyXHere!") && keyx[0] != 0){
			memcpy(n_firm + i, keyx, 16);
		}
		if(*((unsigned int*)(n_firm + i)) == 0xAAAABBBB){
			*((unsigned int*)(n_firm + i)) = (checkEmuNAND() / 0x200) - 1;
		}
	}
	*progress++ = '.'; print("%s", progressbar); ConsoleShow(); ConsolePrevLine();
	sprintf(tmpstr, "%s:%s/0004013800000002.bin", drive, DATAFOLDER);
	if(FileOpen(&tempfile, tmpstr, 1)){
		FileWrite(&tempfile, n_firm, NAT_SIZE, 0);
		FileClose(&tempfile);
		//FileCopy("0004013800000002.bin", tmpstr);
	}else return CONF_ERRNFIRM;
	*progress++ = '.'; print("%s", progressbar); ConsoleShow(); ConsolePrevLine();
	
	//Create AGB patched firmware
	f_read(&firmfile, WORKBUF, AGB_SIZE, &tmpu32);
	u8* a_firm = decryptFirmTitle(WORKBUF, AGB_SIZE, 0x00000202);
	u8* a_firm_patch = GetFilePack("agb_patch.bin");
	if(a_firm){
		applyPatch(a_firm, a_firm_patch);
		sprintf(tmpstr, "%s:%s/0004013800000202.bin", drive, DATAFOLDER);
		if(FileOpen(&tempfile, tmpstr, 1)){
			FileWrite(&tempfile, a_firm, AGB_SIZE, 0);
			FileClose(&tempfile);
		}else return CONF_ERRNFIRM;
		*progress++ = '.';
	}else{	
		//If we cannot decrypt it from firmware.bin becouse of titlekey messed up, it probably means that AGB has been modified in some way.
		//So we read it from his installed ncch...
		FindApp(0x00040138, 0x00000202, 1);
		char* path = getContentAppPath();
		FileOpen(&tempfile, path, 0);
		FileRead(&tempfile, WORKBUF, AGB_SIZE, 0);
		FileClose(&tempfile);
		a_firm = decryptFirmTitleNcch(WORKBUF, AGB_SIZE);
		if(a_firm){
			applyPatch(a_firm, a_firm_patch);
			sprintf(tmpstr, "%s:%s/0004013800000202.bin", drive, DATAFOLDER);
			if(FileOpen(&tempfile, tmpstr, 1)){
				FileWrite(&tempfile, a_firm, AGB_SIZE, 0);
				FileClose(&tempfile);
			}else return CONF_ERRNFIRM;
			*progress++ = '.';
		}else{
			*progress++ = 'x'; //If we get here, then we'll play without AGB, lol
		}
	}
	print("%s", progressbar); ConsoleShow(); ConsolePrevLine();
	
	//Create TWL patched firmware
	f_read(&firmfile, WORKBUF, TWL_SIZE, &tmpu32);
	u8* t_firm = decryptFirmTitle(WORKBUF, TWL_SIZE, 0x00000102);
	u8* t_firm_patch = GetFilePack("twl_patch.bin");
	if(t_firm){
		applyPatch(t_firm, t_firm_patch);
		sprintf(tmpstr, "%s:%s/0004013800000102.bin", drive, DATAFOLDER);
		if(FileOpen(&tempfile, tmpstr, 1)){
			FileWrite(&tempfile, t_firm, TWL_SIZE, 0);
			FileClose(&tempfile);
			//FileCopy("0004013800000102.bin", tmpstr);
		}else return CONF_ERRNFIRM;
		*progress++ = '.'; 
	}else{
		*progress++ = 'x'; 
	}
	print("%s", progressbar); ConsoleShow(); ConsolePrevLine();
	
	sprintf(tmpstr, "%s:%s/data.bin", drive, DATAFOLDER);
	if(FileOpen(&tempfile, tmpstr, 1)){
		FileWrite(&tempfile, __DATE__, 12, 0);
		FileWrite(&tempfile, __TIME__, 9, 12);
		FileClose(&tempfile);
	}else return CONF_CANTOPENFILE;
	*progress++ = '.'; print("%s\n", progressbar); ConsoleShow();
	
	f_close(&firmfile);
	return 0;
}

int CheckInstallationData(){
	File file;
	char str[32];
	if(!FileOpen(&file, "rxtools/data/0004013800000002.bin", 0)) return -1;
	FileClose(&file);
	if(!FileOpen(&file, "rxtools/data/0004013800000202.bin", 0)) return -2;
	FileClose(&file);
	if(!FileOpen(&file, "rxtools/data/0004013800000102.bin", 0)) return -3;
	FileClose(&file);
	if(!FileOpen(&file, "rxtools/data/data.bin", 0)) return -4;
	FileRead(&file, str, 32, 0);
	FileClose(&file);
	if(memcmp(str, __DATE__, 11)) return -5;
	if(memcmp(&str[12], __TIME__, 8)) return -5;
	return 0;
}

void InstallConfigData(){
	if(CheckInstallationData() == 0) return;
	ConsoleInit();
	ConsoleSetTitle("Installation Data Suite");
	
	print("rxTools is installing some data in\nyour SD Card; this is necessary in\norder to speed up many processes and\nto make some features work.\nThis will not take over a minute...\n"); 
	ConsoleShow();
	int res = InstallData("0");	//SD Card
	if(res == 0) print("DONE!\n");
	else print("FAIL! ERR %d\n", res);
	
	print("\nPress A to exit\n"); ConsoleShow();
	WaitForButton(BUTTON_A);
}
