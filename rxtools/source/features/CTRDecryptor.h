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

#ifndef CTR_DECRYPTOR
#define CTR_DECRYPTOR

#include "common.h"

typedef struct{
	u8* buffer;
	u8* keyY;
	u8* ctr;
	size_t size;
	u32 keyslot;
}PartitionInfo;  //This basic struct can be useful anytime, even if i'll add nand decryption/exploring

u32 DecryptPartition(PartitionInfo* info);
void CTRDecryptor();

#endif
