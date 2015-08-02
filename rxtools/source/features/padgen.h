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

#ifndef PADGEN_H
#define PADGEN_H

#include "common.h"

#define MAXENTRIES 1024

typedef struct {
    u8   CTR[16];
    u32  size_mb;
    char filename[180];
} __attribute__((packed)) SdInfoEntry;

typedef struct {
    u32 n_entries;
    SdInfoEntry entries[MAXENTRIES];
} __attribute__((packed, aligned(16))) SdInfo;


typedef struct {
    u8   CTR[16];
    u8   keyY[16];
    u32  size_mb;
    u8   reserved[8];
    u32  uses7xCrypto;
    char filename[112];
} __attribute__((packed)) NcchInfoEntry;

typedef struct {
    u32 padding;
    u32 ncch_info_version;
    u32 n_entries;
    u8  reserved[4];
    NcchInfoEntry entries[MAXENTRIES];
} __attribute__((packed, aligned(16))) NcchInfo;


typedef struct {
    u32  keyslot;
    u32  setKeyY;
    u8   CTR[16];
    u8   keyY[16];
    u32  size_mb;
    char filename[180];
} __attribute__((packed, aligned(16))) PadInfo;

u32 CreatePad(PadInfo *info, int index);
u32 NcchPadgen();
u32 SdPadgen();
void PadGen();

#endif
