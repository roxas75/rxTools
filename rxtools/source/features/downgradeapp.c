#include "downgradeapp.h"
#include "common.h"
#include "screenshot.h"
#include "fs.h"
#include "ff.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "ncch.h"
#include "crypto.h"
#include "TitleKeyDecrypt.h"
#include "NandDumper.h"
#include "aes.h"

typedef struct{
	unsigned int id;
	unsigned short index;
	unsigned short type;
	unsigned char size[8];
	unsigned char signature[0x20];
} tmd_chunck_struct;

char tmpstr[256];
char path[256];
FILINFO curInfo;
DIR myDir;

int FindApp(char* filepath, unsigned int tid_low, unsigned int tid_high, char* drive){
    char* folder = &tmpstr;
    memset(folder, 0, 256);
    DIR* curDir = &myDir;
	FILINFO *myInfo = &curInfo;
    memset((unsigned char*)myInfo, 0, sizeof(FILINFO));
    memset((unsigned char*)curDir, 0, sizeof(DIR));
	myInfo->fname[0] = 'A';
    sprintf(folder, "%s:title/%08X/%08X/content", drive, tid_low, tid_high);
    //print(folder); print("\n"); ConsoleShow();

	if(f_opendir(curDir, folder) != FR_OK) return 0;
	for(int i = 0; myInfo->fname[0] != 0; i++){
		if( f_readdir(curDir, myInfo)) break;
		if(myInfo->fname[0] == '.') continue;
		
		memset(&path, 0, 256);
		File tmp;
        sprintf(path, "%s/%s", folder, myInfo->fname);
		if(strstr(myInfo->fname, ".tmd") || strstr(myInfo->fname, ".TMD")){
			//print(path); print("\n"); ConsoleShow();
			FileOpen(&tmp, path, 0);
			unsigned int size = FileGetSize(&tmp);
			tmd_chunck_struct tmd_entry; memset(&tmd_entry, 0xFF, 0x30);
			int cont = 0;
			while(tmd_entry.index != 0){
				cont++;
				FileRead(&tmp, &tmd_entry, 0x30, size-cont*0x30);
			}
			FileClose(&tmp);
			memset(&path, 0, 256);
			sprintf(path, "%s/%02X%02X%02X%02X.APP", folder, tmd_entry.id&0xFF, tmd_entry.id>>8&0xFF, tmd_entry.id>>16&0xFF, tmd_entry.id>>24&0xFF);
			//print(path); print("\n"); ConsoleShow();
			if(FileOpen(&tmp, path, 0)){
				FileClose(&tmp);
				strcpy(filepath, path);
				f_closedir(curDir);
				return 1;
			}
		}else continue;
	}
	f_closedir(curDir);
	return 0;
}

int checkDgFile(char* path, unsigned int hash){
    unsigned char* buf = (unsigned char*)0x21000000;
    unsigned int rb, fixedsize = 0x00400000;
    File fp;
    if(FileOpen(&fp, path, 0)){
        rb = FileRead(&fp, buf, fixedsize, 0);
        if(!CheckHash(buf, rb, hash)) return 0;
        FileClose(&fp);
    }else{
        return 0;
    }
    return 1;
}

void downgradeMSET(){
    File dg;
    char filepath[256];
    char* dgpath = "0:msetdg.bin";
    unsigned int titleid_low = 0x00040010;
    unsigned int titleid_high[] = { 0x00020000, 0x00021000, 0x00022000}; //JPN, USA, EUR
    unsigned int mset_hash[] = { 0x57358F14, 0xA28EAD9F, 0x530C345B };   //JPN, USA, EUR
    char* regions[] = { "Unknown", "Japan", "USA", "Europe"};
    unsigned int supported_regions = 3;
    unsigned int region = -1;
    ConsoleInit();
    ConsoleSetTitle("MSET Downgrader");
	
    print("Opening MSET app...\n"); ConsoleShow();
    if(FindApp(&filepath, titleid_low, titleid_high[0], "1")) region = 0;
    else if(FindApp(&filepath, titleid_low, titleid_high[1], "1")) region = 1;
    else if(FindApp(&filepath, titleid_low, titleid_high[2], "1")) region = 2;
    if(region != -1){
        print("Region : %s\n\n", regions[region + 1]); ConsoleShow();
		//print(filepath); print("\n"); ConsoleShow(); FileCopy("mset.app", filepath);
        FileOpen(&dg, filepath, 0);
        unsigned int check_val;
        FileRead(&dg, &check_val, 4, 0x130);
        FileClose(&dg);
        if(check_val == 0){
            print("MSET is already in his exploitable\nversion.\nThere's no need to downgrade.\n"); ConsoleShow();
        }else if(checkDgFile(dgpath, mset_hash[region])){
                print("Opening Downgrade Pack...\n"); ConsoleShow();
                FileOpen(&dg, dgpath, 0);
                unsigned int dgsize = FileGetSize(&dg);
                unsigned char* buf = 0x21000000;
                FileRead(&dg, buf, dgsize, 0);
                //Decrypting...
                u8 iv[0x10] = {0};
                u8 Key[0x10] = {0};
                GetTitleKey(&Key, titleid_low, titleid_high[region]);
            	aes_context aes_ctxt;
            	aes_setkey_dec(&aes_ctxt, Key, 0x80);
            	aes_crypt_cbc(&aes_ctxt, AES_DECRYPT, dgsize, iv, buf, buf);
                FileWrite(&dg, buf, dgsize, 0);
                FileClose(&dg);
                //[/Decrypting]

                if(*((unsigned int*)(buf + 0x100)) == 0x4843434E){
                    print("Downgrading...\n"); ConsoleShow();
                    FileCopy(filepath, dgpath);
                    print("Done!\nRemoving Downgrade Pack...\n"); ConsoleShow();
                    f_unlink(dgpath);
                }else{
                    print("Bad Downgrade Pack!\n"); ConsoleShow();
                }
            }else{
                print("Cannot read Downgrade Pack!\n"); ConsoleShow();
            }
    }else{
        print("Cannot read MSET app!\n"); ConsoleShow();
    }
    print("\nPress A to exit\n");
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

void installFBI(){
	char* drive;
	int choice = NandSwitch();
	if(choice == -1) return;
	ConsoleInit();
    ConsoleSetTitle("FBI Installation");
	print("Editing Health&Safety Information...\n"); ConsoleShow();
	char filepath[256];
	memset(filepath, 0, 256);
	if(!FindApp(&filepath, 0x00040010, 0x00020300, choice ? "2" : "1"))	 //JPN
		if(!FindApp(&filepath, 0x00040010, 0x00021300, choice ? "2" : "1")) //USA
			FindApp(&filepath, 0x00040010, 0x00022300, choice ? "2" : "1");  //EUR
	//One of them should actually be found
	//print("%s\n", filepath);
	if(FileCopy(filepath, "0:fbi_inject.app") == 1){
		print("Success!\nDeleting 'fbi_inject.app'...\n");
		f_unlink("0:fbi_inject.app");
	}else
		print("Failure!\n");
	print("\nPress A to exit\n");
	ConsoleShow();
	WaitForButton(BUTTON_A);
}