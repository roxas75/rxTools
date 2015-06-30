#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void* (*rxTools)() = (void*)0x08000030;

void main()
{
	unsigned char* fb1 = (unsigned char*)(0x20184E60);
	unsigned char* fb2 = (unsigned char*)(0x201CB370);
	unsigned char pure_color = 0x77;
	
	*((unsigned int*)0x080FFFC0) = (int)fb1;
	*((unsigned int*)0x080FFFC4) = (int)fb2;
	*((unsigned int*)0x080FFFD8) = 0;
	
	unsigned int* buf = (void*)0x20400000;
	unsigned int base = 0x67893421;
	unsigned int seed = 0x12756342;
	for(int i = 0; i < 400*1024/4; i++){
		buf[i] ^= base;
		base += seed;
	}
	
	if(buf[0] != 0xE51FF004) pure_color = 0x00;
	
	unsigned char*src = (void*)0x20400000;
	unsigned char*dst = (void*)0x08000000;
	for(int i = 0; i < 320*1024; i++){
		dst[i] = src[i];
	}
	
	for(int i = 0; i < 0x46500; i++){
		fb1[i] = pure_color;
		fb2[i] = pure_color;
	}
	
	*(unsigned int*)0x10000020 = 0;
    *(unsigned int*)0x10000020 = 0x340;
	rxTools();
}
