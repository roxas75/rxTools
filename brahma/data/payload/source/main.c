#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void* (*rxTools)() = (void*)0x08000030;

int main()
{
	/*int fbfound = 0;
	unsigned char* screen = 0x20000000;
	for(int i = 0; i < 0x30; i++){
		if( *((unsigned int*)(screen + i + 0)) == 0xABADF00D &&
			*((unsigned int*)(screen + i + 4)) == 0xDEADBEEF	){
			fbfound = 1;
			screen += i;
		}
	}
	if(!fbfound){
		screen = 0x20046500;
		for(int i = 0; i < 0x30; i++){
			if( *((unsigned int*)(screen + i + 0)) == 0xABADF00D &&
				*((unsigned int*)(screen + i + 4)) == 0xDEADBEEF	){
				fbfound = 1;
				screen += i;
			}
		}
	}
	*/
	*((unsigned int*)0x080FFFC0) = 0x20000000; //TOP_SCREEN
	*((unsigned int*)0x080FFFC4) = 0x20046500; //TOP_SCREEN2	
	*((unsigned int*)0x080FFFD0) = 0x2008CA00; //BOT_SCREEN
	*((unsigned int*)0x080FFFD4) = 0x200C4E00; //BOT_SCREEN2
	*((unsigned int*)0x080FFFD8) = 0;          

	unsigned char*src = (void*)0x20400000;
	unsigned char*dst = (void*)0x08000000;
	for(int i = 0; i < 320*1024; i++){
		dst[i] = src[i];
	}

	*(unsigned int*)0x10000020 = 0;
    *(unsigned int*)0x10000020 = 0x340;
	rxTools();
    return 0;
}
