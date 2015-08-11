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

static int rx_strcmp(char* s1, char* s2, unsigned int size, unsigned int w1, unsigned int w2){
	int i;

	for(i = 0; i < size; i++){
		if(s1[i*w1] != s2[i*w2]) return 0;
	}
	return 1;
}

static void rx_strcpy(char* dest, const char* source, unsigned int size, unsigned int w1, unsigned int w2){
	int i;

	for(i = 0; i < size; i++){
		dest[i*w1] = source[i*w2];
	}
}

static inline unsigned int getHID()
{
	return ~*(unsigned int *)0x10146000;
}

static inline void svc_Backdoor(void *addr)
{
    register void *_r0 __asm ("r0") = addr;
    __asm volatile ( "SVC 0x7B" : : "r"(_r0) );
}

//hid
#define BUTTON_A      (1 << 0)
#define BUTTON_B      (1 << 1)
#define BUTTON_SELECT (1 << 2)
#define BUTTON_START  (1 << 3)
#define BUTTON_RIGHT  (1 << 4)
#define BUTTON_LEFT   (1 << 5)
#define BUTTON_UP     (1 << 6)
#define BUTTON_DOWN   (1 << 7)
#define BUTTON_R1     (1 << 8)
#define BUTTON_L1     (1 << 9)
#define BUTTON_X      (1 << 10)
#define BUTTON_Y      (1 << 11)
