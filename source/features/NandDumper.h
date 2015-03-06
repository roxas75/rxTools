#ifndef NAND_DUMPER_H
#define NAND_DUMPER_H
#include "menu.h"

void NandMenu();
void NandDumper();
void DumpNandPartitions();
void GenerateNandXorpads();
void DumpNANDSystemTitles();
void RebuildNand();

static Menu NandOptions = {
		"NAND Options",
		{
			" Create NAND dump", &NandDumper,
			" Decrypt partitions", &DumpNandPartitions,
			" Scan System Titles", &DumpNANDSystemTitles,
			" Generate fat16 Xorpad", &GenerateNandXorpads,
			" Inject EmuNAND partitions", &RebuildNand,		
		},
		5,
		0,
		0
	};

#endif
