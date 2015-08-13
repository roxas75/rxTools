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

#include "common.h"
#include "CTRDecryptor.h"
#include "crypto.h"
#include "fatfs/sdmmc.h"
#include "nand.h"

uint8_t NANDCTR[16];
int sysversion = 0;

int GetSystemVersion() {
	return sysversion;
}
/**CTR offsets with versions*/
const uint8_t *fsVersionCTRs[] = {
	(uint8_t *)0x080D7CAC, //4.x
	(uint8_t *)0x080D858C, //5.x
	(uint8_t *)0x080D748C, //6.x
	(uint8_t *)0x080D740C, //7.x
	(uint8_t *)0x080D74CC, //8.x
	(uint8_t *)0x080D794C, //9.x
	(uint8_t *)0x080D8B8C  //9.x N3DS
};
const uint32_t fsVersionCTRsLength = sizeof(fsVersionCTRs) / sizeof(uint8_t *);
/**Copy NAND Cypto to our region.*/
uint8_t *FSNandFindCTR(void) {
	for (uint32_t i = 0; i < fsVersionCTRsLength; i++)
		if (*(uint32_t *)fsVersionCTRs[i] == 0x5C980) {
			return (uint8_t *)(fsVersionCTRs[i] + 0x30);
		}
	// If value not in previous list start memory scanning (test range)
	for (uint8_t *c = (uint8_t *)0x080D8FFF; c > (uint8_t *)0x08000000; c--)
		if (*(uint32_t *)c == 0x5C980 && *(uint32_t *)(c + 1) == 0x800005C9) {
			return c + 0x30;
		}
	return NULL;
}
/**Copy NAND Cypto to our region.*/
void FSNandInitCrypto(void) {
	uint8_t *ctrStart = FSNandFindCTR();
	uint8_t *ctrStore = NANDCTR;
	uint8_t i = 16; //CTR length
	if (!ctrStart) { return; } //Avoid copying from NULL
	ctrStart = ctrStart + 15;
	//The CTR is stored backwards in memory.
	while (i --) { *(ctrStore++) = *(ctrStart--); }
}

unsigned int checkEmuNAND() {
	uint8_t *check = (uint8_t *)0x26000000;
	int isn3ds = 0;
	if (getMpInfo() == MPINFO_KTR)isn3ds = 1;

	sdmmc_sdcard_readsectors(isn3ds ? 0x4D800000 /0x200 : 0x3AF00000 / 0x200, 1, check);
	if (*((char *)check + 0x100) == 'N' && *((char *)check + 0x101) == 'C' && *((char *)check + 0x102) == 'S' && *((char *)check + 0x103) == 'D') {
		return isn3ds ? 0x4D800000 : 0x3AF00000;
	} else {
		sdmmc_sdcard_readsectors(isn3ds ? 0x76000000 /0x200 : 0x3BA00000 / 0x200, 1, check);
		if (*((char *)check + 0x100) == 'N' && *((char *)check + 0x101) == 'C' && *((char *)check + 0x102) == 'S' && *((char *)check + 0x103) == 'D') {
			return isn3ds ? 0x76000000 : 0x3BA00000;
		} else {
			return 0;
		}
	}
}

void GetNANDCTR(uint8_t *ctr) {
	for (int i = 0; i < 16; i++) { *(ctr + i) = NANDCTR[i]; }
}

void nand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition) {
	PartitionInfo info;
	uint8_t myCtr[16];
	for (int i = 0; i < 16; i++) { myCtr[i] = NANDCTR[i]; }
	info.ctr = myCtr; info.buffer = out; info.size = numsectors * 0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition / 16);
	switch (partition) {
		case TWLN	  : info.keyslot = 0x3; break;
		case TWLP	  : info.keyslot = 0x3; break;
		case AGB_SAVE : info.keyslot = 0x7; break;
		case FIRM0    : info.keyslot = 0x6; break;
		case FIRM1    : info.keyslot = 0x6; break;
		case CTRNAND  : info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no * 0x20);
	sdmmc_nand_readsectors(sector_no + partition / 0x200, numsectors, out);
	DecryptPartition(&info);
}

void nand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition) {
	PartitionInfo info;
	uint8_t myCtr[16];
	for (int i = 0; i < 16; i++) { myCtr[i] = NANDCTR[i]; }
	info.ctr = myCtr; info.buffer = out; info.size = numsectors * 0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition / 16);
	switch (partition) {
		case TWLN	  : info.keyslot = 0x3; break;
		case TWLP	  : info.keyslot = 0x3; break;
		case AGB_SAVE : info.keyslot = 0x7; break;
		case FIRM0    : info.keyslot = 0x6; break;
		case FIRM1    : info.keyslot = 0x6; break;
		case CTRNAND  : info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no * 0x20);
	DecryptPartition(&info);
	sdmmc_nand_writesectors(sector_no + partition / 0x200, numsectors, out);	//Stubbed, i don't wanna risk
}

void emunand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition) {
	PartitionInfo info;
	uint8_t myCtr[16];
	for (int i = 0; i < 16; i++) { myCtr[i] = NANDCTR[i]; }
	info.ctr = myCtr; info.buffer = out; info.size = numsectors * 0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition / 16);
	switch (partition) {
		case TWLN	  : info.keyslot = 0x3; break;
		case TWLP	  : info.keyslot = 0x3; break;
		case AGB_SAVE : info.keyslot = 0x7; break;
		case FIRM0    : info.keyslot = 0x6; break;
		case FIRM1    : info.keyslot = 0x6; break;
		case CTRNAND  : info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no * 0x20);
	sdmmc_sdcard_readsectors(sector_no + partition / 0x200, numsectors, out);
	DecryptPartition(&info);
}

void emunand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition) {
	PartitionInfo info;
	uint8_t myCtr[16];
	for (int i = 0; i < 16; i++) { myCtr[i] = NANDCTR[i]; }
	info.ctr = myCtr; info.buffer = out; info.size = numsectors * 0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition / 16);
	switch (partition) {
		case TWLN	  : info.keyslot = 0x3; break;
		case TWLP	  : info.keyslot = 0x3; break;
		case AGB_SAVE : info.keyslot = 0x7; break;
		case FIRM0    : info.keyslot = 0x6; break;
		case FIRM1    : info.keyslot = 0x6; break;
		case CTRNAND  : info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no * 0x20);
	DecryptPartition(&info);
	sdmmc_sdcard_writesectors(sector_no + partition / 0x200, numsectors, out);	//Stubbed, i don't wanna risk
}
