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

#pragma once

#include <stdint.h>
#include "fatfs/ff.h"

#define File FIL

enum {
	TWLN = 0x00012E00,
	TWLP = 0x09011A00,
	AGB_SAVE = 0x0B100000,
	FIRM0 = 0x0B130000,
	FIRM1 = 0x0B530000,
	CTRNAND = 0x0B95CA00,
	KTR_CTRNAND = 0x0B95AE00,
} NAND_PARTITIONS;

extern int sysver;

void FSNandInitCrypto(void);
unsigned int checkEmuNAND();
void GetNANDCTR(uint8_t *ctr);
void nand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition);
void nand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition);
void emunand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition);
void emunand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition);
