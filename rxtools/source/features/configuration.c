#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "configuration.h"
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

#define DATAFOLDER "rxtools/data"
#define KEYFILENAME "slot0x25KeyX.bin"
#define WORKBUF (u8*)0x21000000
#define NAT_SIZE 0xEBC00
#define AGB_SIZE 0xD9C00
#define TWL_SIZE 0x1A1C00

bool first_boot;
char tmpstr[256] = {0};
char str[100];
File tempfile;
UINT tmpu32;

int InstallData(char* drive){
	FIL firmfile;
	char* progressbar = "[       ]";
	char* progress = progressbar+1;
	print("%s", progressbar);  ConsolePrevLine();
	
	//Create the workdir
	sprintf(tmpstr, "%s:%s", drive, DATAFOLDER);
	f_mkdir(tmpstr);
	
	//Read firmware data
	if (f_open(&firmfile, "firmware.bin", FA_READ | FA_OPEN_EXISTING) != FR_OK) return CONF_NOFIRMBIN;
	*progress++ = '.'; DrawString(BOT_SCREEN, progressbar, 130, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
	
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
	*progress++ = '.'; DrawString(BOT_SCREEN, progressbar, 130, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
	for(int i = 0; i < NAT_SIZE; i+=0x4){
		if(!strcmp((char*)n_firm + i, "InsertKeyXHere!") && keyx[0] != 0){
			memcpy(n_firm + i, keyx, 16);
		}
		if(*((unsigned int*)(n_firm + i)) == 0xAAAABBBB){
			*((unsigned int*)(n_firm + i)) = (checkEmuNAND() / 0x200) - 1;
		}
	}
	*progress++ = '.'; DrawString(BOT_SCREEN, progressbar, 130, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
	sprintf(tmpstr, "%s:%s/0004013800000002.bin", drive, DATAFOLDER);
	if(FileOpen(&tempfile, tmpstr, 1)){
		FileWrite(&tempfile, n_firm, NAT_SIZE, 0);
		FileClose(&tempfile);
	}else {
		f_close(&firmfile);
		return CONF_ERRNFIRM;
	}
	*progress++ = '.'; DrawString(BOT_SCREEN, progressbar, 130, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
	
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
		*progress++ = '.';
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
			*progress++ = '.';
		}else{
			*progress++ = 'x'; //If we get here, then we'll play without AGB, lol
		}
	}
	DrawString(BOT_SCREEN, progressbar, 130, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
	
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
		*progress++ = '.'; 
	}else{
		*progress++ = 'x'; 
	}
	DrawString(BOT_SCREEN, progressbar, 130, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
	
	sprintf(tmpstr, "%s:%s/data.bin", drive, DATAFOLDER);
	if(FileOpen(&tempfile, tmpstr, 1)){
		FileWrite(&tempfile, __DATE__, 12, 0);
		FileWrite(&tempfile, __TIME__, 9, 12);
		FileClose(&tempfile);
	}else {
		f_close(&firmfile);
		return CONF_CANTOPENFILE;
	}
	*progress++ = '.'; DrawString(BOT_SCREEN, progressbar, 130, 50, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
	
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
	
	sprintf(str, "/rxTools/Theme/%c/cfg0TOP.bin", Theme);
	DrawTopSplash(str);
	sprintf(str, "/rxTools/Theme/%c/cfg0.bin", Theme);
	DrawBottomSplash(str);

	int res = InstallData("0");	//SD Card
	sprintf(str, "/rxTools/Theme/%c/cfg1%c.bin", Theme, res == 0 ? 'O' : 'E');
	DrawBottomSplash(str);
	
	InputWait();
}
