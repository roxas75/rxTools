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
#include <features/screenshot.h>
#include <lib/ui/console.h>
#include <lib/ui/draw.h>
#include <lib/hid.h>
#include <lib/fs.h>
#include <lib/fatfs/ff.h>
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
	wchar_t tmp[_MAX_LFN];
	static int top_count = 0;
	static int bot_count = 0;
	unsigned int written = 0;
	char* bmp_cache; char* bmp_ptr;
	uint8_t (*screen_ptr) [SCREEN_HEIGHT][BYTES_PER_PIXEL];

	f_mkdir (L"Screenshot");

	do{
		swprintf(tmp, _MAX_LFN, L"/Screenshot/top_screen_%d.bmp", top_count++);
		if(f_open(&myFile, tmp, FA_READ | FA_OPEN_EXISTING) != FR_OK) break;
		f_close(&myFile);
	}while(1);

	if(f_open(&myFile, tmp, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK){
		unsigned int bmp_size = TOP_SCREEN_WIDTH*SCREEN_HEIGHT*3;
		bmp_cache = (char*)malloc(bmp_size + 0x36);
		bmp_ptr = bmp_cache + 0x36;
		*(unsigned int*)(bmpHeader+0x02) = bmp_size + 0x36;
		*(unsigned int*)(bmpHeader+0x12) = TOP_SCREEN_WIDTH;
		*(unsigned int*)(bmpHeader+0x16) = SCREEN_HEIGHT;
		*(unsigned int*)(bmpHeader+0x22) = bmp_size;
		memcpy(bmp_cache, bmpHeader, 0x36);
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
		swprintf(tmp, _MAX_LFN, L"/Screenshot/bot_screen_%d.bmp", bot_count++);
		if(f_open(&myFile, tmp, FA_READ | FA_OPEN_EXISTING) != FR_OK) break;
		f_close(&myFile);
	}while(1);

	if(f_open(&myFile, tmp, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK){
		unsigned int bmp_size = BOT_SCREEN_WIDTH*SCREEN_HEIGHT*3;
		bmp_cache = (char*)malloc(bmp_size + 0x36);
		bmp_ptr = bmp_cache + 0x36;
		*(unsigned int*)(bmpHeader+0x02) = bmp_size + 0x36;
		*(unsigned int*)(bmpHeader+0x12) = BOT_SCREEN_WIDTH;
		*(unsigned int*)(bmpHeader+0x16) = SCREEN_HEIGHT;
		*(unsigned int*)(bmpHeader+0x22) = bmp_size;
		memcpy(bmp_cache, bmpHeader, 0x36);
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
	uint32_t pad = GetInput();
	if(pad & BUTTON_L1 && pad & BUTTON_R1) ScreenShot();
}
