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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "fatfs/ff.h"
#include "nand.h"
#define File FIL

////////////////////////////////////////////////////////////////Basic FileSystem Operations
bool FSInit(void);
void FSDeInit(void);
bool FileOpen(File *Handle, const char *path, bool truncate);
size_t FileRead(File *Handle, void *buf, size_t size, size_t foffset);
size_t FileWrite(File *Handle, void *buf, size_t size, size_t foffset);
size_t FileGetSize(File *Handle);
void FileClose(File *Handle);
////////////////////////////////////////////////////////////////Advanced FileSystem Operations
uint32_t FSFileCopy(char *target, char *source);
