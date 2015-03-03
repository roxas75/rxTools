#ifndef NAND_DUMPER_H
#define NAND_DUMPER_H
#include "menu.h"

void NandMenu();
void NandDumper();
void EmuNandDumper();
void DumpNandPartitions();
void DumpEmuNandPartitions();
void GenerateNandXorpads();
void DumpNANDSystemTitles();
void DumpEmuNANDSystemTitles();

static Menu NandOptions = {	
		"Nand Options", 
		{	
			" Create EmuNAND dump", &EmuNandDumper,
			" Create NAND dump", &NandDumper,
			" Decrypt EmuNAND partitions", &DumpEmuNandPartitions,
			" Decrypt NAND partitions", &DumpNandPartitions,
			" Dump EmuNAND System Titles", &DumpEmuNANDSystemTitles,
			" Dump NAND System Titles", &DumpNANDSystemTitles,
			" Generate fat16 Xorpad", &GenerateNandXorpads,			
		},
		7,
		0,
		0
	};

#endif