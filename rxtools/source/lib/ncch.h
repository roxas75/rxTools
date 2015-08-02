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

#ifndef NCCH_H
#define NCCH_H

typedef enum
{
	NCCHTYPE_EXHEADER = 1,
	NCCHTYPE_EXEFS = 2,
	NCCHTYPE_ROMFS = 3,
} ctr_ncchtypes;

typedef struct
{
	unsigned char signature[0x100];
	unsigned char magic[4];
	unsigned char contentsize[4];
	unsigned char partitionid[8];
	unsigned char makercode[2];
	unsigned char version[2];
	unsigned char reserved0[4];
	unsigned char programid[8];
	unsigned char tempflag;
	unsigned char reserved1[0x2f];
	unsigned char productcode[0x10];
	unsigned char extendedheaderhash[0x20];
	unsigned char extendedheadersize[4];
	unsigned char reserved2[4];
	unsigned char flags[8];
	unsigned char plainregionoffset[4];
	unsigned char plainregionsize[4];
	unsigned char reserved3[8];
	unsigned char exefsoffset[4];
	unsigned char exefssize[4];
	unsigned char exefshashregionsize[4];
	unsigned char reserved4[4];
	unsigned char romfsoffset[4];
	unsigned char romfssize[4];
	unsigned char romfshashregionsize[4];
	unsigned char reserved5[4];
	unsigned char exefssuperblockhash[0x20];
	unsigned char romfssuperblockhash[0x20];
} ctr_ncchheader;

void ncch_get_counter(ctr_ncchheader header, unsigned char counter[16], unsigned char type);
unsigned int getle32(const unsigned char* p);
#endif
