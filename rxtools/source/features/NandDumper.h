#ifndef NAND_DUMPER_H
#define NAND_DUMPER_H
#include "menu.h"

int NandSwitch();
void NandDumper();
void DumpNandPartitions();
void GenerateNandXorpads();
void DumpNANDSystemTitles();
void RebuildNand();

#endif
