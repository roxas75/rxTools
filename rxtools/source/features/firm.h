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
#include "fatfs/ff.h"

typedef enum {
	TID_HI_FIRM = 0x00040138
} TitleIdHi;

typedef enum {
        TID_CTR_NATIVE_FIRM = 0x00000002,
        TID_CTR_TWL_FIRM = 0x00000102,
        TID_CTR_AGB_FIRM = 0x00000202,
        TID_KTR_NATIVE_FIRM = 0x20000002
} TitleIdLo;

extern const char firmPathFmt[];
extern const char firmPatchPathFmt[];

int PastaMode();
void FirmLoader();
void rxModeWithSplash(int emu);
int rxMode(int emu);
uint8_t* decryptFirmTitleNcch(uint8_t* title, unsigned int size);
uint8_t *decryptFirmTitle(uint8_t *title, unsigned int size, uint8_t key[16]);
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
