// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
#include "lib.h"
#include "FS.h"
#include <wchar.h>
#include <stdio.h>
#include <stdint.h>

#ifdef MEMDUMP
unsigned char handle[32];

static void memdump(wchar_t* filename, unsigned char* buf, unsigned int size){
	unsigned int br;
	memset(VRAM, 0x77, 0x600000);			//Grey flush : Start Dumping
	memset(&handle, 0, 32);
	fopen9(&handle, filename, 6);
	fwrite9(&handle, &br, buf, size);
	fclose9(&handle);
	memset(VRAM, 0xFF, 0x600000);			//White flush : Finished Dumping
}
#endif

static int32_t originalcode[] = { 0xE3550000, 0xE3A01001, 0xE1A00011, 0x0A000003 };
static int32_t patchcode[] = { 0xE3A00001, 0xE8BD8070 };
static int32_t* dest = (void*)0x20000400;
static void patchregion(){
	for(int i = 0; i < 8 / sizeof(int32_t); i++) dest[i] = patchcode[i];
}	

static void patch_processes(){
	const uintptr_t mset = 0x24000000;
	const uintptr_t menu = 0x26A00000;
	for(int i = 0; i < 0x600000; i++){
		//System Menu code, which locks the region
		if(dest == (void*)0x20000400){	//This means we haven't still found our code
			if( ((*((int32_t *)(menu + i + 0x0)) == originalcode[0]) &&
				((*((int32_t *)(menu + i + 1))) == originalcode[1]) &&
				((*((int32_t *)(menu + i + 2))) == originalcode[2]) &&
				((*((int32_t *)(menu + i + 3))) == originalcode[3]))){	
				dest = (int32_t *)(menu + i);    //Basically, once we found where the code is, there is no point on searching it again
				break;
			}
		}
		//System Settings label
		if(rx_strcmp((char *)mset - i, "Ver.", 4, 2, 1)){
			rx_strcpy((char *)mset - i, "Shit", 4, 2, 1);
		}
	}
}

void myThread(){
	while(1){
#ifdef MEMDUMP
		if(getHID() & BUTTON_SELECT){
			memdump(L"sdmc:/FCRAM.bin", 0x20000000, 0x10000);
		}
#endif
		patch_processes();
		if(*dest != patchcode[0])
			svc_Backdoor(&patchregion);		//Edit just if the code is not patched, or the arm9 will get mad
	}
	__asm("SVC 0x09");
}
