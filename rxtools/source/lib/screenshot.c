#include "screenshot.h"
#include "common.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "fs.h"
#include "ff.h"
#include "stdio.h"

unsigned char bmpTopHeader[] = {	
	0x42, 0x4D, 0x36, 0x65, 0x04, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 
	0x00, 0x00, 0x90, 0x01, 0x00, 0x00, 0xF0, 0x00, 
	0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x65, 0x04, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

void ScreenShot(){
	File myFile;
	char tmp[256]; int count = 0;
	f_mkdir ("Screenshot");  
	do{
		sprintf(tmp, "/Screenshot/top_screen_%d.bmp", count++);
	}while(FileOpen(&myFile, tmp, 0));
	
	if(FileOpen(&myFile, tmp, 1)){
		FileWrite(&myFile, bmpTopHeader, 0x36, 0);
		int pos = 0x36;
		for(int i = SCREEN_HEIGHT-1; i >= 0; i--){
			for(int j = 0; j < SCREEN_WIDTH; j++){
				int color = GetPixel(TOP_SCREEN, j, i);
				FileWrite(&myFile, &color, 0x4, pos);
				pos += 3;
			}
		}
		FileClose(&myFile);
	}
}

void TryScreenShot(){
/*	u32 pad = */GetInput();
	//if(pad & BUTTON_L1 && pad & BUTTON_R1) ScreenShot();
	//Disabled, i don't need any screenshot for now, but the function is here
}

