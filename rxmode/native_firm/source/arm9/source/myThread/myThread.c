// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
#include "lib.h"
#include "FS.h"
#include <wchar.h>
#include <stdio.h>

u8 handle[32];

void memdump(wchar_t *filename, u8 *buf, u32 size) {
	u32 br;
	for (int i = 0; i < 0x600000; i++) {
		*(VRAM + i) = 0x77;			//Grey flush : Start Dumping
	}
	rx_memset(&handle, 0, 32);
	fopen9(&handle, filename, 6);
	fwrite9(&handle, &br, buf, size);
	fclose9(&handle);
	for (int i = 0; i < 0x600000; i++) {
		*(VRAM + i) = 0xFF;			//White flush : Finished Dumping
	}
}
static u8 originalcode[] = { 0x00, 0x00, 0x55, 0xE3, 0x01, 0x10, 0xA0, 0xE3, 0x11, 0x00, 0xA0, 0xE1, 0x03, 0x00, 0x00, 0x0A };
static u8 patchcode[] = { 0x01, 0x00, 0xA0, 0xE3, 0x70, 0x80, 0xBD, 0xE8 };
static u8 *dest = (u8 *)0x20000400;
void patchregion() {
	for (int i = 0; i < 8; i++) { *(dest + i) = patchcode[i]; }
}

void patch_processes() {
	char *mset = (char *)0x24000000;
	u8 *menu = (u8 *)0x26A00000;
	for (int i = 0; i < 0x600000; i += 4) {
		//System Menu code, which locks the region
		if (dest == (u8 *)0x20000400) {	//This means we haven't still found our code
			if ((*((u32 *)(menu + i + 0x0)) == *((u32 *)&originalcode[0x0])) &&
			    (*((u32 *)(menu + i + 0x4)) == *((u32 *)&originalcode[0x4])) &&
			    (*((u32 *)(menu + i + 0x8)) == *((u32 *)&originalcode[0x8])) &&
			    (*((u32 *)(menu + i + 0xC)) == *((u32 *)&originalcode[0xC]))) {
				dest = menu + i;    //Basically, once we found where the code is, there is no point on searching it again
				break;
			}
		}
		//System Settings label
		if (rx_strcmp(mset - i, "Ver.", 4, 2, 1)) {
			rx_strcpy(mset - i, "Shit", 4, 2, 1);
		}
	}
}

void myThread() {
	while (1) {
		/*if(getHID() & BUTTON_SELECT){
			memdump(L"sdmc:/FCRAM.bin", 0x20000000, 0x10000);
		}*/
		patch_processes();
		if (*((u32 *)dest) != *((u32 *)&patchcode[0])) {
			svc_Backdoor(&patchregion);    //Edit just if the code is not patched, or the arm9 will get mad
		}
	}
	__asm("SVC 0x09");
}
