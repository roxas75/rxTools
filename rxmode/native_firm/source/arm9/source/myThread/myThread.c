// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
#include "lib.h"
#include "FS.h"
#include <wchar.h>
#include <stdio.h>

u8 handle[32];

void rx_memdump(wchar_t *filename, u8 *buf, u32 size) {
	u32 br = 0;
	//Flush grey: Dumping start.
	rx_memset(VRAM, 0x77, 0x600000);
	rx_memset(&handle, 0, 32);
	fopen9(&handle, filename, 6);
	fwrite9(&handle, &br, buf, size);
	fclose9(&handle);
	rx_memset(VRAM, 0xFF, 0x600000);
}
char originalcode[] = { 0x00, 0x00, 0x55, 0xE3, 0x01, 0x10, 0xA0, 0xE3, 0x11, 0x00, 0xA0, 0xE1, 0x03, 0x00, 0x00, 0x0A };
char patchcode[] = { 0x01, 0x00, 0xA0, 0xE3, 0x70, 0x80, 0xBD, 0xE8 };
char *dest = (char *)0x20000400;
/** Patch System Menu code for region free. Indeed just memcpy. */
void RegionLockFree(void) {
	char *destp = dest, *patchp = patchcode;
	u32 i = sizeof(patchcode);
	while (i--) *(destp++) = *(patchp++);
}
/** Look up the System Menu code for region lock code.
  * @retval If find the target code, return 1. otherwise, 0.
  */
u8 RegionLockSearch(void) {
	char *mset = (char *)0x24000000;
	char *menu = (char *)0x26A00000;
	//System Menu code, which locks the region
	dest = rx_memmem(originalcode, menu, sizeof(originalcode)/sizeof(u8), 0x600000);
	char *msetlabel =  menu - dest + mset;
	if (dest) {
		//System Settings label
		if (rx_strcmp(msetlabel, "Ver.", 4, 2, 1)) {
			rx_strcpy(msetlabel, "Shit", 4, 2, 1);
		}
		return 1;
	} else return 0;
}

void myThread() {
	while (1) {
		/*if(getHID() & BUTTON_SELECT){
			rx_memdump(L"sdmc:/FCRAM.bin", 0x20000000, 0x10000);
		}*/
		if (RegionLockSearch()) {
			if (*((u32 *)dest) != *((u32 *)&patchcode[0])) {
				svc_Backdoor(&RegionLockFree);    //Edit just if the code is not patched, or the arm9 will get mad
			}
		}
	}
	__asm("SVC 0x09");
}
