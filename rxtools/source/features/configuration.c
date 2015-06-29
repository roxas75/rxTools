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

#define DATAFOLDER "rxtools/data"
#define KEYFILENAME "slot0x25KeyX.bin"
#define WORKBUF (u8*)0x21000000

#define rxmode_emu_label "RX3D"
#define rxmode_sys_label "Ver."

char tmpstr[256] = {0};
File tmpfile;

int InstallData(char* drive){
	char* progressbar = "[       ]";
	char* progress = progressbar+1;
	print("%s", progressbar); ConsoleShow(); ConsolePrevLine();
	//Create the workdir
	sprintf(tmpstr, "%s:%s", drive, DATAFOLDER);
	f_mkdir(tmpstr);
	f_chmod(tmpstr, AM_HID, AM_HID);
	
	//Read firmware data
	if(FileOpen(&tmpfile, "firmware.bin", 0)){
		FileRead(&tmpfile, WORKBUF, 0x600000, 0);
		FileClose(&tmpfile);
	}else return CONF_NOFIRMBIN;
	*progress++ = '.'; print("%s", progressbar); ConsoleShow(); ConsolePrevLine();
	
	//Create patched native_firm
	u8* n_firm = decryptFirmTitle(WORKBUF, 0xF0000, 0x00000002);
	u8* n_firm_patch = GetFilePack("nat_patch.bin");
	applyPatch(n_firm, n_firm_patch);
	u8 keyx[16] = {0};
	if(GetSystemVersion() < 3){
		FileOpen(&tmpfile, KEYFILENAME, 0);
		FileRead(&tmpfile, &keyx[0], 16, 0);
		FileClose(&tmpfile);
	}
	*progress++ = '.'; print("%s", progressbar); ConsoleShow(); ConsolePrevLine();
	for(int i = 0; i < 0xF0000; i+=0x4){
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
	if(FileOpen(&tmpfile, tmpstr, 1)){
		FileWrite(&tmpfile, n_firm, 0xF0000, 0);
		FileClose(&tmpfile);
		//FileCopy("0004013800000002.bin", tmpstr);
	}else return CONF_ERRNFIRM;
	*progress++ = '.'; print("%s", progressbar); ConsoleShow(); ConsolePrevLine();
	
	//Create AGB patched firmware
	u8* a_firm = decryptFirmTitle(WORKBUF+0x200000, 0xD9C00, 0x00000202);
	u8* a_firm_patch = GetFilePack("agb_patch.bin");
	applyPatch(a_firm, a_firm_patch);
	sprintf(tmpstr, "%s:%s/0004013800000202.bin", drive, DATAFOLDER);
	if(FileOpen(&tmpfile, tmpstr, 1)){
		FileWrite(&tmpfile, a_firm, 0xD9000, 0);
		FileClose(&tmpfile);
		//FileCopy("0004013800000202.bin", tmpstr);
	}else return CONF_ERRNFIRM;
	*progress++ = '.'; print("%s", progressbar); ConsoleShow(); ConsolePrevLine();
	
	//Create TWL patched firmware
	u8* t_firm = decryptFirmTitle(WORKBUF+0x400000, 0x1A1C00, 0x00000102);
	u8* t_firm_patch = GetFilePack("twl_patch.bin");
	applyPatch(t_firm, t_firm_patch);
	sprintf(tmpstr, "%s:%s/0004013800000102.bin", drive, DATAFOLDER);
	if(FileOpen(&tmpfile, tmpstr, 1)){
		FileWrite(&tmpfile, t_firm, 0x1A1000, 0);
		FileClose(&tmpfile);
		//FileCopy("0004013800000102.bin", tmpstr);
	}else return CONF_ERRNFIRM;
	*progress++ = '.'; print("%s", progressbar); ConsoleShow(); ConsolePrevLine();
	
	sprintf(tmpstr, "%s:%s/data.bin", drive, DATAFOLDER);
	if(FileOpen(&tmpfile, tmpstr, 1)){
		FileWrite(&tmpfile, __DATE__, 12, 0);
		FileWrite(&tmpfile, __TIME__, 9, 12);
		FileClose(&tmpfile);
	}else return CONF_CANTOPENFILE;
	*progress++ = '.'; print("%s\n", progressbar); ConsoleShow();
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
	else print("FAIL! ERR %d", res);
	
	print("\nPress A to exit\n"); ConsoleShow();
	WaitForButton(BUTTON_A);
}