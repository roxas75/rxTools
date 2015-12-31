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

#ifndef CFW_H
#define CFW_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <reboot.h>
#include <lib/fatfs/ff.h>
#include <lib/crypto.h>

typedef struct {
	uint8_t keyX[AES_BLOCK_SIZE];
	uint8_t keyY[AES_BLOCK_SIZE];
	uint8_t ctr[AES_BLOCK_SIZE];
	char size[8];
	uint8_t pad[8];
	uint8_t control[AES_BLOCK_SIZE];
	union {
		uint32_t pad[8];
		struct {
			uint8_t unk[16];
			uint8_t keyX_0x16[AES_BLOCK_SIZE];
		} s;
	} ext;
} Arm9Hdr;

extern const char firmPathFmt[];
extern const char firmPatchPathFmt[];

int PastaMode();
void FirmLoader();
void rxModeWithSplash(int emu);
int rxMode(int emu);
uint8_t* decryptFirmTitleNcch(uint8_t* title, size_t *size);
uint8_t *decryptFirmTitle(uint8_t *title, size_t size, size_t *firmSize, uint8_t key[16]);
FRESULT applyPatch(void *file, const char *patch);

static inline int getFirmPath(char *s, TitleIdLo id)
{
	return sprintf(s, firmPathFmt, TID_HI_FIRM, id);
}

static inline int getFirmPatchPath(char *s, TitleIdLo id)
{
	return sprintf(s, firmPatchPathFmt, TID_HI_FIRM, id);
}

#endif
