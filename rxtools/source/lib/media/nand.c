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

#include <lib/mpcore.h>
#include <features/decryption.h>
#include <lib/crypto.h>
#include <lib/media/tmio.h>
#include <lib/media/nand.h>

uint8_t NANDCTR[16];
int sysver;

// Counter offsets for old 3DS
static const uintptr_t fsCountersCtr[] = {
	0x080D7CAC, //4.x
	0x080D858C, //5.x
	0x080D748C, //6.x
	0x080D740C, //7.x
	0x080D74CC, //8.x
	0x080D794C, //9.x
};

// The counter offset of 9.x on new 3DS
static const uintptr_t fsCounter9Ktr = 0x080D8B8C;

static void *findCounter(void)
{
	uint32_t i;

	if (getMpInfo() == MPINFO_KTR) {
		if (*(uint32_t *)fsCounter9Ktr == 0x5C980) {
			sysver = 9;
			return (void *)(fsCounter9Ktr + 0x30);
		}
	} else {
		for (i = 0; i < sizeof(fsCountersCtr) / sizeof(uintptr_t); i++)
			if (*(uint32_t *)fsCountersCtr[i] == 0x5C980) {
				sysver = i + 3;
				return (void *)(fsCountersCtr[i] + 0x30);
			}
	}

	// If value not in previous list start memory scanning (test range)
	for (i = 0x080D8FFC; i > 0x08000000; i -= sizeof(uint32_t))
		if (((uint32_t *)i)[0] == 0x5C980 && ((uint32_t *)i)[1] == 0x800005C9)
			return (void *)(i + 0x30);

	return NULL;
}

/**Copy NAND Cypto to our region.*/
void FSNandInitCrypto(void) {
	uint8_t *ctrStart = findCounter();
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

	tmio_readsectors(TMIO_DEV_SDMC, isn3ds ? 0x4D800000 /0x200 : 0x3AF00000 / 0x200, 1, check);
	if (*((char *)check + 0x100) == 'N' && *((char *)check + 0x101) == 'C' && *((char *)check + 0x102) == 'S' && *((char *)check + 0x103) == 'D') {
		return isn3ds ? 0x4D800000 : 0x3AF00000;
	} else {
		tmio_readsectors(TMIO_DEV_SDMC, isn3ds ? 0x76000000 /0x200 : 0x3BA00000 / 0x200, 1, check);
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

static void cryptNand(enum nandPart part, uint32_t sector, void *p, uint32_t n)
{
	PartitionInfo info;
	uint8_t ctr[AES_BLOCK_SIZE];

	GetNANDCTR(ctr);
	aesAddCtr(ctr, part / 16 + sector * 0x20);
	info.ctr = ctr;
	info.buffer = p;
	info.size = n * 0x200;
	info.keyY = NULL;
	switch (part) {
		case TWLN:
		case TWLP:
			info.keyslot = 0x3;
			break;

		case AGB_SAVE:
			info.keyslot = 0x7;
			break;

		case FIRM0:
		case FIRM1:
			info.keyslot = 0x6;
			break;

		case CTRNAND:
			info.keyslot = 0x4;
			break;

		case KTR_CTRNAND:
			info.keyslot = 0x5;
			break;
	}

	DecryptPartition(&info);
}

void nand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition) {
	if (partition == CTRNAND && getMpInfo() == MPINFO_KTR) partition = KTR_CTRNAND; //SWITCH TO KTR_CTRNAND IF ON N3DS
	tmio_readsectors(TMIO_DEV_NAND, sector_no + partition / 0x200, numsectors, out);
	cryptNand(partition, sector_no, out, numsectors);
}

void nand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition) {
	if (partition == CTRNAND && getMpInfo() == MPINFO_KTR) partition = KTR_CTRNAND; //SWITCH TO KTR_CTRNAND IF ON N3DS
	cryptNand(partition, sector_no, out, numsectors);
	tmio_writesectors(TMIO_DEV_NAND, sector_no + partition / 0x200, numsectors, out);	//Stubbed, i don't wanna risk
}

void emunand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition) {
	if (partition == CTRNAND && getMpInfo() == MPINFO_KTR) partition = KTR_CTRNAND; //SWITCH TO KTR_CTRNAND IF ON N3DS
	tmio_readsectors(TMIO_DEV_SDMC, sector_no + partition / 0x200, numsectors, out);
	cryptNand(partition, sector_no, out, numsectors);
}

void emunand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition) {
	if (partition == CTRNAND && getMpInfo() == MPINFO_KTR) partition = KTR_CTRNAND; //SWITCH TO KTR_CTRNAND IF ON N3DS
	cryptNand(partition, sector_no, out, numsectors);
	tmio_writesectors(TMIO_DEV_SDMC, sector_no + partition / 0x200, numsectors, out);	//Stubbed, i don't wanna risk
}
