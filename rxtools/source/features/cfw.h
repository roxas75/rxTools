#ifndef CFW_H
#define CFW_H

#include "common.h"


typedef enum {
	PLATFORM_3DS=1,
	PLATFORM_N3DS=7,
} Platform_UnitType;

void DevMode();
void rxModeSys();
void rxModeEmu();
void rxModeQuickBoot();
u8* decryptFirmTitleNcch(u8* title, unsigned int size);
u8* decryptFirmTitle(u8* title, unsigned int size, unsigned int tid, int drive);
void applyPatch(unsigned char* file, unsigned char* patch);
Platform_UnitType Platform_CheckUnit(void);

#endif
