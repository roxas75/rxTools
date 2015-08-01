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

#ifndef FS_H
#define FS_H
#include <stdio.h>
extern unsigned int fopen9(void *handle, wchar_t* name, unsigned int flag);
extern void fwrite9(void* handle, unsigned int* bytesWritten, void* dst, unsigned int size);
extern void fread9(void* handle, unsigned int* bytesRead, void *src, unsigned int size);
extern void fclose9(void *handle);

#endif
