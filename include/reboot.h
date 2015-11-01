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

#ifndef REBOOT_H
#define REBOOT_H

#include <stdint.h>
#include <elf.h>

#define FIRM_SEG_NUM (4)

#define FIRM_PATH_FMT "rxTools/data/%08X%08X.bin"
#define FIRM_PATCH_PATH_FMT PATCHES_PATH "/%08X%08X.elf"

#define REBOOT_CTX ((RebootCtx *)0x24000000)

typedef struct {
	uint32_t offset;
	uint32_t addr;
	uint32_t size;
	uint32_t isArm11;
	uint8_t hash[32];
} FirmSeg;

typedef struct {
	uint32_t magic;
	uint32_t unused0;
	uint32_t arm11Entry;
	uint32_t arm9Entry;
	uint8_t unused1[48];
	FirmSeg segs[FIRM_SEG_NUM];
	uint8_t sig[256];
} FirmHdr;

typedef struct {
	uint8_t keyx[16];
	union {
		uint32_t u32;
		char c[4];
	} label;
	uint32_t sector;
} PatchCtx;

typedef struct {
	union {
		FirmHdr hdr;
		uint8_t b[0x100000];
	} firm;
	union {
		Elf32_Ehdr hdr;
		uint8_t b[0x2000];
	} patch;
} RebootCtx;

#endif
