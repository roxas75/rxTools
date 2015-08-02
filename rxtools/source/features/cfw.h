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

#include "common.h"


typedef enum {
	PLATFORM_3DS=1,
	PLATFORM_N3DS=7,
} Platform_UnitType;

void DevMode();
void rxModeSys();
void rxModeEmu();
void rxModeQuickBoot();
u8* decryptFirmTitleNcch(u8* title, unsigned int size);
u8* decryptFirmTitle(u8* title, unsigned int size, unsigned int tid, int drive);
void applyPatch(unsigned char* file, unsigned char* patch);
Platform_UnitType Platform_CheckUnit(void);

#endif
