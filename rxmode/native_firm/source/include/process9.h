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

#ifndef PROCESS9_H
#define PROCESS9_H
#include <stdint.h>

typedef uint32_t P9File[8];

static inline void p9FileInit(P9File f)
{
	unsigned int i;

	for (i = 0; i < sizeof(P9File); i += sizeof(uint32_t)) {
		*f = 0;
		f++;
	}
}

unsigned int p9Open(P9File, const wchar_t *name, unsigned int flag);
void p9Write(P9File, size_t *bytesWritten, const void *src, size_t size);
void p9Read(P9File, size_t *bytesRead, void *dst, size_t size);
void p9Close(P9File);
int p9RecvPxi(void);

#endif
