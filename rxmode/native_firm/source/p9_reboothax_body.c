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

#include <stddef.h>
#include <stdint.h>
#include <reboot.h>
#include <elf.h>
#include <ctx.h>

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

static void SET_MPU_PERM_RWFORALL(uint32_t *v, uint8_t area)
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

static void drainWriteBuffer()
{
	__asm__ volatile ("mcr p15, 0, %0, c7, c10, 4\n" :: "r"(0));
}

static void cleanDcacheLine(void *p)
{
	__asm__ volatile ("mcr p15, 0, %0, c7, c10, 1\n" :: "r"(p));
}

static void flushIcacheLine(void *p)
{
	__asm__ volatile ("mcr p15, 0, %0, c7, c5, 1\n" :: "r"(p));
}

static void setupMpu()
{
	uint32_t i;

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

static void *memcpy32(void *dst, const void *src, size_t n)
{
	const uint32_t *_src;
	uint32_t *_dst;
	uintptr_t btm;

	_dst = dst;
	_src = src;
	btm = (uintptr_t)dst + n;
	while ((uintptr_t)_dst < btm) {
		*_dst = *_src;
		_dst++;
		_src++;
	}

	return _dst;
}

static void loadFirm()
{
	const FirmSeg *seg;
	unsigned int i;

	seg = REBOOT_CTX->firm.hdr.segs;
	for (i = 0; i < FIRM_SEG_NUM; i++) {
		memcpy32((void *)seg->addr, REBOOT_CTX->firm.b + seg->offset, seg->size);
		seg++;
	}
}

static int memcmp(const void *s1, const void *s2, size_t n)
{
	int d;

	while (n > 0) {
		d = *(unsigned char *)s1 - *(unsigned char *)s2;
		if (d)
			return d;

		s1 = (unsigned char *)s1 + 1;
		s2 = (unsigned char *)s2 + 1;
		n--;
	}

	return 0;
}

static void patchFirm()
{
	static const char patchNandPrefix[] = ".patch.p9.nand";
#ifndef PLATFORM_KTR
	static const char patchKeyxStr[] = ".patch.p9.keyx";
#endif
	Elf32_Shdr *shdr, *btm;
	const char *shstrtab, *sh_name;

	shdr = (void *)(REBOOT_CTX->patch.b + REBOOT_CTX->patch.hdr.e_shoff);
	shstrtab = REBOOT_CTX->patch.b + shdr[REBOOT_CTX->patch.hdr.e_shstrndx].sh_offset;
	for (btm = shdr + REBOOT_CTX->patch.hdr.e_shnum; shdr != btm; shdr++) {
		if (!(shdr->sh_flags & SHF_ALLOC) || shdr->sh_type != SHT_PROGBITS)
			continue;

		sh_name = shstrtab + shdr->sh_name;
		if ((patchCtx.sector == 0 && !memcmp(sh_name, patchNandPrefix, sizeof(patchNandPrefix) - 1))
#ifndef PLATFORM_KTR
			|| (patchCtx.keyx[0] == 0 && !memcmp(sh_name, patchKeyxStr, sizeof(patchKeyxStr)))
#endif
			)
		{
			continue;
		}

		memcpy32((void *)shdr->sh_addr,
			REBOOT_CTX->patch.b + shdr->sh_offset,
			shdr->sh_size);
	}
}

static void flushFirmData()
{
	uintptr_t dstCur, dstBtm;
	const FirmSeg *seg;
	unsigned int i;

	seg = REBOOT_CTX->firm.hdr.segs;
	for (i = 0; i < FIRM_SEG_NUM; i++) {
		dstCur = seg->addr;
		for (dstBtm = seg->addr + seg->size; dstCur < dstBtm; dstCur += 32)
			cleanDcacheLine((void *)dstCur);

		seg++;
	}

	drainWriteBuffer();
}

static void flushFirmInstr()
{
	uintptr_t dstCur, dstBtm;
	const FirmSeg *seg;
	unsigned int i;

	seg = REBOOT_CTX->firm.hdr.segs;
	for (i = 0; i < FIRM_SEG_NUM; i++) {
		dstCur = seg->addr;
		for (dstBtm = seg->addr + seg->size; dstCur < dstBtm; dstCur += 32)
			if (!seg->isArm11)
				flushIcacheLine((void *)dstCur);

		seg++;
	}
}

static void arm11Enter(uint32_t *arm11EntryDst)
{
	*arm11EntryDst = REBOOT_CTX->firm.hdr.arm11Entry;
	cleanDcacheLine(arm11EntryDst);
	drainWriteBuffer();
}

static _Noreturn void arm9Enter()
{
	__asm__ volatile ("b 0x801B01C\n");
	__builtin_unreachable();
}

_Noreturn void __attribute__((section(".text.start")))
rebootFunc(const PatchCtx *ctx, uint32_t *arm11EntryDst)
{
	setupMpu();
	loadFirm();
	memcpy32(&patchCtx, ctx, sizeof(patchCtx));
	patchFirm();
	flushFirmData();
	arm11Enter(arm11EntryDst);
	flushFirmInstr();
	arm9Enter();
}
