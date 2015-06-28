#include "common.h"
#include "CTRDecryptor.h"
#include "crypto.h"
#include "sdmmc.h"
#include "nand.h"

u8 NANDCTR[16];
int sysversion = 0;

int GetSystemVersion() {
	return sysversion;
}
/**CTR offsets with versions*/
const u8 *fsVersionCTRs[] = {
	(u8 *)0x080D7CAC, //4.x
	(u8 *)0x080D858C, //5.x
	(u8 *)0x080D748C, //6.x
	(u8 *)0x080D740C, //7.x
	(u8 *)0x080D74CC, //8.x
	(u8 *)0x080D794C //9.x
};
const u32 fsVersionCTRsLength = sizeof(fsVersionCTRs) / sizeof(u8 *);
/**Copy NAND Cypto to our region.*/
u8 *FSNandFindCTR(void) {
	for (u32 i = 0; i < fsVersionCTRsLength; i++)
		if (*(u32 *)fsVersionCTRs[i] == 0x5C980) {
			return (u8 *)(fsVersionCTRs[i] + 0x30);
		}
	// If value not in previous list start memory scanning (test range)
	for (u8 *c = (u8 *)0x080D8FFF; c > (u8 *)0x08000000; c--)
		if (*(u32 *)c == 0x5C980 && *(u32 *)(c + 1) == 0x800005C9) {
			return c + 0x30;
		}
	return NULL;
}
/**Copy NAND Cypto to our region.*/
void FSNandInitCrypto(void) {
	u8 *ctrStart = FSNandFindCTR();
	u8 *ctrStore = NANDCTR;
	u8 i = 16; //CTR length
	if (!ctrStart) { return; } //Avoid copying from NULL
	ctrStart = ctrStart + 15;
	//The CTR is stored backwards in memory.
	while (i --) { *(ctrStore++) = *(ctrStart--); }
}

int checkEmuNAND() {
	u8 *check = (u8 *)0x26000000;
	sdmmc_sdcard_readsectors(0x3AF00000 / 0x200, 1, check);
	if (*((char *)check + 0x100) == 'N' && *((char *)check + 0x101) == 'C' && *((char *)check + 0x102) == 'S' && *((char *)check + 0x103) == 'D') {
		return 0x3AF00000;
	} else {
		sdmmc_sdcard_readsectors(0x3BA00000 / 0x200, 1, check);
		if (*((char *)check + 0x100) == 'N' && *((char *)check + 0x101) == 'C' && *((char *)check + 0x102) == 'S' && *((char *)check + 0x103) == 'D') {
			return 0x3BA00000;
		} else {
			return 0;
		}
	}
}

void GetNANDCTR(u8 *ctr) {
	for (int i = 0; i < 16; i++) { *(ctr + i) = NANDCTR[i]; }
}

void nand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition) {
	PartitionInfo info;
	u8 myCtr[16];
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
	u8 myCtr[16];
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
	u8 myCtr[16];
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
	u8 myCtr[16];
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
