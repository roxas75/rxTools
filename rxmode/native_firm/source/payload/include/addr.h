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

#ifndef ADDR_H
#define ADDR_H

#define ARM9_FCRAM_ADDR 0x20000000

#define ARM9_FOPEN_ADDR 0x0805B181
#define ARM9_FWRITE_ADDR 0x0805C4D1
#define ARM9_FREAD_ADDR 0x0804D9B1
#define ARM9_FCLOSE_ADDR 0x0805B26D

#define ARM9_REGION_TOP_ADDR 0x26A00000
#define ARM9_REGION_BTM_ADDR 0x27000000

#define ARM9_MSET_TOP_PADDR 0x23A00000
#define ARM9_MSET_BTM_PADDR 0x24000000

#define ARM9_ARM11EXCVEC_ADDR 0x1FFF4000

#define ARM9_FIQ_HANDLER_PTR_ADDR 0x08000008
#define ARM9_INSTR_HANDLER_PTR_ADDR 0x08000018
#define ARM9_PREFETCH_HANDLER_PTR_ADDR 0x08000020
#define ARM9_DATA_HANDLER_PTR_ADDR 0x0800002C

#define ARM11_EXCVEC_VADDR 0xFFFF0000

#endif
