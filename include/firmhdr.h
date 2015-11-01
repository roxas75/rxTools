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

#ifndef FIRMHDR_H
#define FIRMHDR_H

#include <stdint.h>

#define FIRM_SEG_NUM (4)

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

#endif
