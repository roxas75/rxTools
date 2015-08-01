/*
 * Copyright (C) 2015 The PASTA Team
 * Originally written by Roxas75
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

#include "lib.h"
#include "FS.h"
#include <wchar.h>
#include <stdio.h>

unsigned char handle[32];

void memdump(wchar_t* filename, unsigned char* buf, unsigned int size){
	unsigned int br;
	for(int i = 0; i < 0x600000; i++){
		*(VRAM + i) = 0x77;			//Grey flush : Start Dumping
	}
	memset(&handle, 0, 32);
	fopen9(&handle, filename, 6);
	fwrite9(&handle, &br, buf, size);
	fclose9(&handle);
	for(int i = 0; i < 0x600000; i++){
		*(VRAM + i) = 0xFF;			//White flush : Finished Dumping
	}
}
static unsigned char originalcode[] = { 0x00, 0x00, 0x55, 0xE3, 0x01, 0x10, 0xA0, 0xE3, 0x11, 0x00, 0xA0, 0xE1, 0x03, 0x00, 0x00, 0x0A };
static unsigned char patchcode[] = { 0x01, 0x00, 0xA0, 0xE3, 0x70, 0x80, 0xBD, 0xE8 };
static char* dest = (void*)0x20000400;
void patchregion(){
	for(int i = 0; i < 8; i++) *(dest + i) = patchcode[i];
}

void patch_processes(){
	char* mset = (void*)0x24000000;
	char* menu = (void*)0x26A00000;
	for(int i = 0; i < 0x600000; i+=4){
		//System Menu code, which locks the region
		if(dest == (void*)0x20000400){	//This means we haven't still found our code
			if( (*((unsigned int*)(menu + i + 0x0)) == *((unsigned int*)&originalcode[0x0])) &&
				(*((unsigned int*)(menu + i + 0x4)) == *((unsigned int*)&originalcode[0x4])) &&
				(*((unsigned int*)(menu + i + 0x8)) == *((unsigned int*)&originalcode[0x8])) &&
				(*((unsigned int*)(menu + i + 0xC)) == *((unsigned int*)&originalcode[0xC]))){
				dest = menu + i;    //Basically, once we found where the code is, there is no point on searching it again
				break;
			}
		}
		//System Settings label
		if(rx_strcmp(mset - i, "Ver.", 4, 2, 1)){
			rx_strcpy(mset - i, "Shit", 4, 2, 1);
		}
	}
}

void myThread(){
	while(1){
		/*if(getHID() & BUTTON_SELECT){
			memdump(L"sdmc:/FCRAM.bin", 0x20000000, 0x10000);
		}*/
		patch_processes();
		if(*((unsigned int*)dest) != *((unsigned int*)&patchcode[0]))
			svc_Backdoor(&patchregion);		//Edit just if the code is not patched, or the arm9 will get mad
	}
	__asm("SVC 0x09");
}
