#ifndef CFW_H
#define CFW_H

#include "common.h"

void DevMode();
void rxModeSys();
void rxModeEmu();
u8* decryptFirmTitle(u8* title, unsigned int size, unsigned int tid);
void applyPatch(unsigned char* file, unsigned char* patch);

#endif
