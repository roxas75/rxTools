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

#include <stdlib.h>
#include <string.h>
#include "screenshot.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "fs.h"
#include "fatfs/ff.h"
#include "stdio.h"

void ScreenShot(){
	unsigned char bmpHeader[] = {
		0x42, 0x4D, 0x36, 0x65, 0x04, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
		0x00, 0x00, 0x90, 0x01, 0x00, 0x00, 0xF0, 0x00,
		0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x65, 0x04, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};

	File myFile;
	char tmp[256];
	static int top_count = 0;
	static int bot_count = 0;
	unsigned int written = 0;
	char* bmp_cache; char* bmp_ptr;
	uint8_t (*screen_ptr) [SCREEN_HEIGHT][BYTES_PER_PIXEL];

	f_mkdir ("Screenshot");

	do{
		sprintf(tmp, "/Screenshot/top_screen_%d.bmp", top_count++);
		if(f_open(&myFile, tmp, FA_READ | FA_OPEN_EXISTING) != FR_OK) break;
		f_close(&myFile);
	}while(1);

	if(f_open(&myFile, tmp, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK){
		unsigned int bmp_size = TOP_SCREEN_WIDTH*SCREEN_HEIGHT*3;
		bmp_cache = (char*)malloc(bmp_size + 0x36);
		bmp_ptr = bmp_cache;
		*(unsigned int*)(bmpHeader+0x02) = bmp_size + 0x36;
		*(unsigned int*)(bmpHeader+0x12) = TOP_SCREEN_WIDTH;
		*(unsigned int*)(bmpHeader+0x16) = SCREEN_HEIGHT;
		*(unsigned int*)(bmpHeader+0x22) = bmp_size;
		f_write(&myFile, bmpHeader, 0x36, &written);
		screen_ptr = (void*)TOP_SCREEN;
		for(int y = 0; y < SCREEN_HEIGHT; y++){
			for(int x = 0; x < TOP_SCREEN_WIDTH; x++){
				memcpy(bmp_ptr, screen_ptr[x][y], 3);
				bmp_ptr += 3;
			}
		}
		f_write(&myFile, bmp_cache, bmp_size + 0x36, &written);
		free(bmp_cache);
		f_close(&myFile);
	}

	do{
		sprintf(tmp, "/Screenshot/bot_screen_%d.bmp", bot_count++);
		if(f_open(&myFile, tmp, FA_READ | FA_OPEN_EXISTING) != FR_OK) break;
		f_close(&myFile);
	}while(1);

	if(f_open(&myFile, tmp, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK){
		unsigned int bmp_size = BOT_SCREEN_WIDTH*SCREEN_HEIGHT*3;
		bmp_cache = (char*)malloc(bmp_size + 0x36);
		bmp_ptr = bmp_cache;
		*(unsigned int*)(bmpHeader+0x02) = bmp_size + 0x36;
		*(unsigned int*)(bmpHeader+0x12) = BOT_SCREEN_WIDTH;
		*(unsigned int*)(bmpHeader+0x16) = SCREEN_HEIGHT;
		*(unsigned int*)(bmpHeader+0x22) = bmp_size;
		f_write(&myFile, bmpHeader, 0x36, &written);
		screen_ptr = (void*)BOT_SCREEN;
		for(int y = 0; y < SCREEN_HEIGHT; y++){
			for(int x = 0; x < BOT_SCREEN_WIDTH; x++){
				memcpy(bmp_ptr, screen_ptr[x][y], 3);
				bmp_ptr += 3;
			}
		}
		f_write(&myFile, bmp_cache, bmp_size + 0x36, &written);
		free(bmp_cache);
		f_close(&myFile);
	}

}

void TryScreenShot(){
/*	uint32_t pad = */GetInput();
	//if(pad & BUTTON_L1 && pad & BUTTON_R1) ScreenShot();
	//Disabled, i don't need any screenshot for now, but the function is here
}
