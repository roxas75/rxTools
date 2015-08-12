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

#ifndef MEMORY_H
#define MEMORY_H

#define CTR_ARM9_VRAM_ADDR 0x18000000
#define KTR_ARM9_VRAM_ADDR 0xDEADBEEF

#ifdef PLATFORM_KTR
#define ARM9_VRAM_ADDR KTR_ARM9_VRAM_ADDR
#else
#define ARM9_VRAM_ADDR CTR_ARM9_VRAM_ADDR
#endif

#define CTR_ARM11_VRAM_PADDR CTR_ARM9_VRAM_ADDR
#define KTR_ARM11_VRAM_PADDR KTR_ARM9_VRAM_ADDR

#define ARM11_VRAM_PADDR ARM9_VRAM_ADDR

#endif
