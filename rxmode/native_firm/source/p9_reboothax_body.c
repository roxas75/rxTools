/*
 * Copyright (C) 2015 The PASTA Team
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

#include <stdint.h>
#include <firmhdr.h>
#include <memory.h>

#define SET_MPU_REGION(id, base, size, enable)	{	\
	__asm__ volatile ("mcr p15, 0, %0, c6, c" #id ", 0\n"	\
		:: "r"(((base) & 0xFFFFF000) | ((size) & 0x3E) | ((enable) ? 1 : 0)));	\
	}

#define PERM_DATA "2"
#define PERM_INSTR "3"
#define READ_MPU_PERM(v, type)	{	\
	__asm__("mrc p15, 0, %0, c5, c0, " type "\n" : "=r"(*(v)));	\
}
#define WRITE_MPU_PERM(v, type)	{	\
	__asm__("mcr p15, 0, %0, c5, c0, " type "\n" :: "r"(v));	\
}

static void __attribute__((section(".patch.p9.reboot.body")))
SET_MPU_PERM_RWFORALL(uint32_t *v, uint8_t area)
{
	const uint8_t bits = 4;
	uint8_t shift;

	shift = area * bits;
	// 3 means the area is readable and writable for everyone.
	*v = (*v & ~(((1 << bits) - 1) << shift)) | (3 << shift);
}

#define MPU_DCACHE "0"
#define MPU_ICACHE "1"
#define FETCH "c2"
#define WRITE "c3"
#define READ_MPU_CACHABLE(v, id, type) {	\
	__asm__("mrc p15, 0, %0, " type ", c0, " id "\n" : "=r"(*(v)));	\
}
#define WRITE_MPU_CACHABLE(v, id, type) {	\
	__asm__ volatile ("mcr p15, 0, %0, " type ", c0, " id "\n" :: "r"(v));	\
}

static void __attribute__((section(".patch.p9.reboot.body")))
drainEntireDcache()
{
	__asm__ volatile ("mcr p15, 0, %0, c7, c6, 4\n" :: "r"(0));
}

static void __attribute__((section(".patch.p9.reboot.body")))
cleanDcacheLine(uintptr_t p)
{
	__asm__ volatile ("mcr p15, 0, %0, c7, c10, 1\n" :: "r"(p));
}

static void __attribute__((section(".patch.p9.reboot.body")))
flushIcacheLine(uintptr_t p)
{
	__asm__ volatile ("mcr p15, 0, %0, c7, c5, 1\n" :: "r"(p));
}

static void __attribute__((section(".patch.p9.reboot.body")))
setupMpu()
{
	uint32_t i;

	drainEntireDcache();
	SET_MPU_REGION(3, 0x10000000, 52, 1);

	READ_MPU_PERM(&i, PERM_DATA);
	SET_MPU_PERM_RWFORALL(&i, 4);
	WRITE_MPU_PERM(i, PERM_DATA);

	READ_MPU_PERM(&i, PERM_INSTR);
	SET_MPU_PERM_RWFORALL(&i, 4);
	WRITE_MPU_PERM(i, PERM_INSTR);

	SET_MPU_REGION(4, 0x18000000, 52, 1);

	READ_MPU_CACHABLE(&i, MPU_DCACHE, FETCH);
	WRITE_MPU_CACHABLE(i | (1 << 4) | (1 << 5), MPU_DCACHE, FETCH);

	READ_MPU_CACHABLE(&i, MPU_ICACHE, FETCH);
	WRITE_MPU_CACHABLE(i | (1 << 4) | (1 << 5), MPU_ICACHE, FETCH);

	READ_MPU_CACHABLE(&i, MPU_DCACHE, WRITE);
	WRITE_MPU_CACHABLE(i | (1 << 4) | (1 << 5), MPU_DCACHE, WRITE);
}

static void __attribute__((section(".patch.p9.reboot.body")))
loadFirm()
{
	uintptr_t srcTop, srcCur, dstTop, dstBtm, dstCur;
	const FirmSeg *seg;
	unsigned int i;

	seg = ((FirmHdr *)FIRM_ADDR)->segs;
	for (i = 0; i < FIRM_SEG_NUM; i++) {
		srcTop = FIRM_ADDR + seg->offset;
		dstTop = seg->addr;
		dstBtm = dstTop + seg->size;

		srcCur = srcTop;
		dstCur = dstTop;
		while (dstCur < dstBtm) {
			*(uint32_t *)dstCur = *(uint32_t *)srcCur;
			srcCur += sizeof(uint32_t);
			dstCur += sizeof(uint32_t);
		}

		for (dstCur = dstTop; dstCur < dstBtm; dstCur += 32) {
			cleanDcacheLine(dstCur);

			if (!seg->isArm11)
				flushIcacheLine(dstCur);
		}

		seg++;
	}
}

static void __attribute__((section(".patch.p9.reboot.body")))
arm11Enter()
{
	const uintptr_t arm11Entry = 0x1FFFFFFC;

	*(uint32_t *)arm11Entry = ((FirmHdr *)FIRM_ADDR)->arm11Entry;
	cleanDcacheLine(arm11Entry);
}

static _Noreturn void __attribute__((section(".patch.p9.reboot.body")))
arm9Enter()
{
	__asm__ volatile ("b 0x801B01C\n");
	__builtin_unreachable();
}

_Noreturn void __attribute__((section(".patch.p9.reboot.body.top")))
rebootFunc()
{
	// Enter system mode and disable IRQ and FIQ
	__asm__ volatile ("msr CPSR_c, #0xDF\n");

	setupMpu();
	loadFirm();
	arm11Enter();
	arm9Enter();
}
