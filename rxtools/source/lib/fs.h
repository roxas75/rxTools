#pragma once

#include "common.h"
#include "ff.h"
#define File FIL

enum {
	TWLN = 0x00012E00,
	TWLP = 0x09011A00,
	AGB_SAVE = 0x0B100000,
	FIRM0 = 0x0B130000,
	FIRM1 = 0x0B530000,
	CTRNAND = 0x0B95CA00,
} NAND_PARTITIONS;
////////////////////////////////////////////////////////////////Basic FileSystem Operations
bool InitFS();
void DeinitFS();
bool FileOpen(File *Handle, const char *path, bool truncate);
size_t FileRead(File *Handle, void *buf, size_t size, size_t foffset);
size_t FileWrite(File *Handle, void *buf, size_t size, size_t foffset);
size_t FileGetSize(File *Handle);
void FileClose(File *Handle);
////////////////////////////////////////////////////////////////Advanced FileSystem Operations
int FileCopy(char *dest, char *source);
u32 FSFileCopy(char *target, char *source);
////////////////////////////////////////////////////////////////NAND Operations
int GetSystemVersion();
void FSNandInitCrypto(void);
int checkEmuNAND();
void GetNANDCTR(u8 *ctr);
void nand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition);
void nand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition);
void emunand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition);
void emunand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition);
