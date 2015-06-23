// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
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

void patch_label(char* pstr){
	for(char *str = 0x24000000; str > 0x23A00000; str-=4)
		if(rx_strcmp(str, "Ver.", 4, 2, 1))
			rx_strcpy(str, pstr, 4, 2, 1);
}

void myThread(){
	while(1){
		if(getHID() & BUTTON_SELECT){
			//memdump(L"sdmc:/FCRAM.bin", 0x20000000, 0x08000000);
		}
		patch_label("Shit");
	}
	__asm("SVC 0x09");
}
