#include "NandDumper.h"
#include "common.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "fs.h"
#include "screenshot.h"

#define TITLE "NAND Dumper\n"
#define NAND_SIZE 0x3AF00000
#define NAND_SECTOR_SIZE 0x200
#define BUF1 0x21000000

char myString[256];		//for showing percentages

void NandDumper(){
	File myFile;
	ConsoleInit();
	ConsoleAddText(TITLE);
	unsigned char* buf = 0x21000000;
	unsigned int nsectors = 0x200;  //sectors in a row
	char ProgressBar[] = "[                            ]"; 
	unsigned int progress = 1;
	int BACKCOLOR = ConsoleGetBackgroundColor();
	if(FileOpen(&myFile, "/NAND.bin", 1)){
		ConsoleAddText("Dumping...\n"); ConsoleShow();
		int x, y; ConsoleGetXY(&x, &y); y += CHAR_WIDTH * 6; x += CHAR_WIDTH*2; 
		DrawString(TOP_SCREEN, ProgressBar, x, y, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
		DrawString(TOP_SCREEN, "Press B anytime to abort", x, y + CHAR_WIDTH*2, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
		
		for(int count = 0; count < NAND_SIZE/NAND_SECTOR_SIZE/nsectors; count++){
			sdmmc_nand_readsectors(count*nsectors, nsectors, buf);
			FileWrite(&myFile, buf, nsectors*NAND_SECTOR_SIZE, count*NAND_SECTOR_SIZE*nsectors);
			TryScreenShot();
			if((count % (int)(NAND_SIZE/NAND_SECTOR_SIZE/nsectors/25)) == 0 && count != 0){
				DrawString(TOP_SCREEN, "-", x+(CHAR_WIDTH*(progress++)), y, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
			}
			unsigned int pad = GetInput();
			if(pad & BUTTON_B) break;
		}
		
		FileClose(&myFile);
		ConsoleAddText("\nFinished dumping!"); ConsoleShow();
	}else{
		ConsoleAddText("Failed to create the dump"); ConsoleShow();
	}
	ConsoleAddText("\nPress A to exit"); ConsoleShow();
	WaitForButton(BUTTON_A);
}

void DumpNandPartitions(){
	char* p_name[] = { "twln.bin", "twlp.bin", "agb_save.bin", "firm0.bin", "firm1.bin", "ctrnand.bin" };
	unsigned int p_size[] = { 0x08FB5200, 0x020B6600, 0x00030000, 0x00400000, 0x00400000, 0x2F3E3600};
	unsigned int p_addr[] = { TWLN, TWLP, AGB_SAVE, FIRM0, FIRM1, CTRNAND };
	int sect_row = 0x80;
	
	ConsoleInit();
	ConsoleAddText("NAND Partitions Decryptor\n \n");
	for(int i = 0; i < 6; i++){
		File out;
		FileOpen(&out, p_name[i], 1);
		sprintf(myString, "Dumping %s ...", p_name[i]);
		ConsoleAddText(myString); ConsoleShow();
		for(int j = 0; j*0x200 < p_size[i]; j += sect_row){
			sprintf(myString, "%08X / %08X", j*0x200, p_size[i]);
			int x, y; ConsoleGetXY(&x, &y); y += CHAR_WIDTH * 4; x += CHAR_WIDTH*2; 
			DrawString(TOP_SCREEN, myString, x, y, ConsoleGetTextColor(), ConsoleGetBackgroundColor());
			
			nand_readsectors(j, sect_row, BUF1, p_addr[i]);
			FileWrite(&out, BUF1, sect_row*0x200, j*0x200);
		}
		FileClose(&out);
	}
	ConsoleAddText("\nPress A to exit"); ConsoleShow();
	WaitForButton(BUTTON_A);
}
