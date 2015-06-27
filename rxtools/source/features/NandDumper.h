#ifndef NAND_DUMPER_H
#define NAND_DUMPER_H
#include "menu.h"

enum nand_type{
	UNK_NAND = -1,
	SYS_NAND = 1,
	EMU_NAND = 2
};

int NandSwitch();
void NandDumper();
void DumpNandPartitions();
void GenerateNandXorpads();
void DumpNANDSystemTitles();
void RebuildNand();

#endif
