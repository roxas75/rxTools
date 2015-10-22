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

#include <wchar.h>
#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#ifndef PLATFORM_KTR
#include <FS.h>
#include <handlers.h>
#include "hookswi.h"
#include "font.c"
#endif
#include "vars.h"
#include "lib.c"

#ifdef DEBUG_DUMP_FCRAM
static void memdump(wchar_t *filename, unsigned char *buf, size_t size)
{
#ifndef PLATFORM_KTR
	unsigned int handle[8];
	unsigned int i;

	for (i = 0; i < 0x600000; i++)
		((char *)ARM9_VRAM_ADDR)[i] = 0x77;			//Grey flush : Start Dumping

	for (i = 0; i < sizeof(handle) / sizeof(unsigned int); i++)
		handle[i] = 0;

	fopen9(handle, filename, 6);
	fwrite9(handle, &i, buf, size);
	fclose9(handle);

	for (i = 0; i < 0x600000; i++)
		((char *)ARM9_VRAM_ADDR)[i] = 0xFF;			//White flush : Finished Dumping
#endif
}
#endif

static int patchLabel()
{
	uintptr_t p;

#ifdef PLATFORM_KTR
	for (p = 0x27500000; p < 0x27b00000; p++)
#else
	for (p = 0x23A00000; p < 0x24000000; p++)
#endif
	{
		//System Settings label
		if(rx_strcmp((char *)p, "Ver.", 4, 2, 1)){
			rx_strcpy((char*)p, label, 4, 2, 1);
			return 0;
		}
	}

	return 1;
}

#ifndef PLATFORM_KTR

#define FB_HEIGHT 240
#define FB_WIDTH 320
#define FNT_HEIGHT 8
#define FNT_WIDTH 8

static int cur_x;
static int cur_y;

static void initScr()
{
	cur_x = 0;
	cur_y = FB_HEIGHT;
}

static void scrPutc(int c)
{
	struct px {
		unsigned char b;
		unsigned char g;
		unsigned char r;
	};

	struct px (* const fb)[FB_HEIGHT] = (void *)ARM9_VRAM_ADDR;
	unsigned int fnt_x, fnt_y;
	const unsigned char *p;

	switch(c) {
		case '\t':
			cur_x += FNT_WIDTH * 4;
			break;

		case '\n':
			cur_x = 0;
			cur_y -= FNT_HEIGHT;
			if (cur_y < 0)
				cur_y = FB_HEIGHT;
			break;

		default:
			p = font + (c - 32) * FNT_WIDTH;

			for (fnt_x = 0; fnt_x < FNT_WIDTH; fnt_x++) {
				for (fnt_y = 0; fnt_y < FNT_HEIGHT; fnt_y++) {
					if ((0x80 >> fnt_y) & *p) {
						fb[cur_x][cur_y - fnt_y].r = 255;
						fb[cur_x][cur_y - fnt_y].g = 255;
						fb[cur_x][cur_y - fnt_y].b = 255;
					} else {
						fb[cur_x][cur_y - fnt_y].r = 0;
						fb[cur_x][cur_y - fnt_y].g = 0;
						fb[cur_x][cur_y - fnt_y].b = 0;
					}
				}

				p++;
				cur_x++;
			}
	}
}

static void scrPuts(const char *s)
{
	while (*s) {
		scrPutc(*s);
		s++;
	}
}

static void scrItoa(unsigned val, int base, int w)
{
        char buf[10];
        int i = 0;
        int rem;

        do {
                rem = val % base;
                buf[i++] = (rem < 10 ? 0x30 : 0x37) + rem;
                val /= base;
                w--;
        } while (val || w > 0);

        while (i > 0)
                scrPutc(buf[--i]);
}

static int getArmBoff(void *p)
{
	int i;

	i = *(int *)p & 0xFFFFFF;
	if (i & 0x800000)
		i -= 0x1000000;

	return i << 2;
}

static void writeArmB(void *cur, const void *dst)
{
	*(int *)cur = (0xE << 28) | (0x5 << 25) | (0x0 << 24)
		| (((uintptr_t)dst - ((uintptr_t)cur + 8)) >> 2);
}

#define REG_NUM 17

_Noreturn void mainHandler(int regs[REG_NUM], const char *type)
{
	static const char regNames[REG_NUM][4] = {
		"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
		"R8", "R9", "R10", "R11", "R12", "SP", "LR", "PC", "CPSR"
	};
	void * const arm11Swi = (void *)0x1FFF4008;
	unsigned int * const arm11Code = (void *)0x1FFF4B20;
	unsigned int *p, *q;
	unsigned int i, origCpsr;

	// Back up CPSR
	__asm__ ("mrs %0, cpsr" : "=r"(origCpsr));

	while (1) {
		// Change the mode to supervisor
		i = (origCpsr & ~0xF) | 0x13;
		__asm__ volatile ("msr cpsr_c, %0" :: "r"(i));

		// Allow to write to ARM11 exception vectors
		i = 0x10000037;
		__asm__ volatile ("mcr p15, 0, %0, c6, c3, 0" :: "r"(i));

		// Restore CPSR
		__asm__ volatile ("msr cpsr_c, %0" :: "r"(origCpsr));

		p = arm11Code;
		for (q = hookSwi; q != hookSwiBtm; q++) {
			*p = *q;
			p++;
		}

		writeArmB(arm11Swi, arm11Code);

		initScr();
		scrPuts("\nException Information by rxTools\n\n\n\n"
			"CPU: ARM9\tType: ");
		scrPuts(type);

		scrPuts("\n\n\nRegisters:\n");

		for (i = 0; i < REG_NUM; i++) {
			scrPutc(i % 2 ? '\t' : '\n');
			scrPuts(regNames[i]);
			scrPuts(": 0x");
			scrItoa(regs[i], 16, 8);
		}
	}
}

static void initExceptionHandler()
{
	*(void **)0x08000008 = handleFiq;
	*(void **)0x08000018 = handleInstr;
	*(void **)0x0800002C = handleData;
	*(void **)0x08000020 = handlePrefetch;
}

#endif

void myThread(){
#ifndef PLATFORM_KTR
	initExceptionHandler();
#endif

	while (1) {
#ifdef DEBUG_DUMP_FCRAM
		if(getHID() & BUTTON_SELECT){
			memdump(L"sdmc:/FCRAM.bin", (void *)0x20000000, 0x10000);
		}
#endif
#ifdef DEBUG_DATA_ABORT
		if (getHID() & BUTTON_START)
			*(int *)8192 = 100;
#endif
		if (!patchLabel()) {
#if !defined(DEBUG_DATA_ABORT) && !defined(DEBUG_DUMP_FCRAM)
			__asm__ volatile ("svc #9");
#endif
		}
	}
}
