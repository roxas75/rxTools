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

void *memset(void * ptr, int value, unsigned int num){
    unsigned char *p = ptr;
    while (num) {
        *p++ = value;
        num--;
    }
    return ptr;
}

int rx_strcmp(char* s1, char* s2, unsigned int size, unsigned int w1, unsigned int w2){
	for(int i = 0; i < size; i++){
		if(s1[i*w1] != s2[i*w2]) return 0;
	}
	return 1;
}

void rx_strcpy(char* dest, char* source, unsigned int size, unsigned int w1, unsigned int w2){
	for(int i = 0; i < size; i++){
		dest[i*w1] = source[i*w2];
	}
}

void rx_hextostr(unsigned int num, char* str) {
	int i;
    char *ptr = str + 8;
    for(i = 0; i < 8; i++){
		int x = (num & 0xF);
		char y = '0';
		if(x > 0x9) y +=7;
        *--ptr = y + x;
        num >>= 4;
    }
}

int memcmp(void* buf1, void* buf2, int size){
	int equal = 0;
	for(int i = 0; i < size; i++){
		if(*((unsigned char*)buf1 + i) != *((unsigned char*)buf2 + i)){
			equal = i;
			break;
		}
	}
	return equal;
}

unsigned int getHID(){
	return ~*(unsigned int*)0x10146000;
}
