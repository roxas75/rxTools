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

#ifndef FIRMHEADERS_H
#define FIRMHEADERS_H

#include <stdint.h>

typedef struct
{
	uint32_t offset;
	uint32_t address;
	uint32_t size;
	uint32_t type;
	uint8_t hash[32];
} firm_sectionheader;

typedef struct
{
	uint32_t magic;
	uint8_t reserved1[4];
	uint32_t entrypointarm11;
	uint32_t entrypointarm9;
	uint8_t reserved2[0x30];
	firm_sectionheader section[4];
	uint8_t signature[0x100];
} firm_header;

typedef struct
{
	uint8_t signature[0x100];
	uint8_t magic[4];
	uint32_t contentsize;
	uint8_t partitionid[8];
	uint8_t makercode[2];
	uint8_t version[2];
	uint8_t reserved0[4];
	uint8_t programid[8];
	uint8_t reserved1[0x10];
	uint8_t logohash[0x20];
	uint8_t productcode[0x10];
	uint8_t extendedheaderhash[0x20];
	uint32_t extendedheadersize;
	uint8_t reserved2[4];
	uint8_t flags[8];
	uint32_t plainregionoffset;
	uint32_t plainregionsize;
	uint32_t logooffset;
	uint32_t logosize;
	uint32_t exefsoffset;
	uint32_t exefssize;
	uint32_t exefshashregionsize;
	uint8_t reserved4[4];
	uint32_t romfsoffset;
	uint32_t romfssize;
	uint32_t romfshashregionsize;
	uint8_t reserved5[4];
	uint8_t exefssuperblockhash[0x20];
	uint8_t romfssuperblockhash[0x20];
} ncch_header;

typedef struct
{
	uint8_t reserved[5];
	uint8_t flag;
	uint16_t remasterversion;
} exheader_systeminfoflags;

typedef struct
{
	uint32_t address;
	uint32_t nummaxpages;
	uint32_t codesize;
} exheader_codesegmentinfo;

typedef struct
{
	uint8_t name[8];
	exheader_systeminfoflags flags;
	exheader_codesegmentinfo text;
	uint32_t stacksize;
	exheader_codesegmentinfo ro;
	uint8_t reserved[4];
	exheader_codesegmentinfo data;
	uint32_t bsssize;
} exheader_codesetinfo;

typedef struct
{
	uint8_t programid[0x30][8];
} exheader_dependencylist;

typedef struct
{
	uint8_t savedatasize[8];
	uint8_t jumpid[8];
	uint8_t reserved2[0x30];
} exheader_systeminfo;

typedef struct
{
	uint8_t extsavedataid[8];
	uint8_t systemsavedataid[8];
	uint8_t accessibleuniqueids[8];
	uint8_t accessinfo[7];
	uint8_t otherattributes;
} exheader_storageinfo;

typedef struct
{
	uint8_t programid[8];
	uint32_t coreversion;
	uint8_t reserved0[2];
	uint8_t flag;
	uint8_t priority;
	uint8_t resourcelimitdescriptor[0x10][2];
	exheader_storageinfo storageinfo;
	uint8_t serviceaccesscontrol[0x20][8];
	uint8_t reserved[0x1f];
	uint8_t resourcelimitcategory;
} exheader_arm11systemlocalcaps;

typedef struct
{
	uint32_t descriptors[28];
	uint8_t reserved[0x10];
} exheader_arm11kernelcapabilities;

typedef struct
{
	uint8_t descriptors[15];
	uint8_t descversion;
} exheader_arm9accesscontrol;

typedef struct
{
	// systemcontrol info {
	//   coreinfo {
	exheader_codesetinfo codesetinfo;
	exheader_dependencylist deplist;
	//   }
	exheader_systeminfo systeminfo;
	// }
	// accesscontrolinfo {
	exheader_arm11systemlocalcaps arm11systemlocalcaps;
	exheader_arm11kernelcapabilities arm11kernelcaps;
	exheader_arm9accesscontrol arm9accesscontrol;
	// }
	struct {
		uint8_t signature[0x100];
		uint8_t ncchpubkeymodulus[0x100];
		exheader_arm11systemlocalcaps arm11systemlocalcaps;
		exheader_arm11kernelcapabilities arm11kernelcaps;
		exheader_arm9accesscontrol arm9accesscontrol;
	} accessdesc;
} exheader_header;

typedef struct
{
	uint8_t name[8];
	uint32_t offset;
	uint32_t size;
} exefs_sectionheader;


typedef struct
{
	exefs_sectionheader section[8];
	uint8_t reserved[0x80];
	uint8_t hashes[8][0x20];
} exefs_header;

#endif // FIRMHEADERS_H
