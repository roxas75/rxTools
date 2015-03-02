#ifndef MY_MENU
#define MY_MENU

#include "menu.h"
#include "CTRDecryptor.h"
#include "NandDumper.h"
#include "TitleKeyDecrypt.h"
#include "padgen.h"

static Menu MainMenu = {	
		"Roxas75 3DS Toolkit [RX v1.1]", 
		{	
			" Decrypt CTR Titles", &CTRDecryptor,
			" Create NAND dump", &NandDumper,
			" Decrypt Title Keys", &DecryptTitleKeys,
			" Dump Tickets", &DumpTicketDb,
			" Generate Xorpads", &PadGen,
			" Decrypt NAND partitions", &DumpNandPartitions,
		},
		6,
		0,
		0
	};

#endif