#ifndef MY_MENU
#define MY_MENU

#include "menu.h"
#include "CTRDecryptor.h"
#include "NandDumper.h"
#include "TitleKeyDecrypt.h"
#include "padgen.h"

static Menu MainMenu = {	
		"Roxas75 3DS Toolkit [RX v1.2]", 
		{	
			" Decrypt CTR Titles", &CTRDecryptor,
			" Decrypt Title Keys", &DecryptTitleKeys,
			" Dump Tickets", &DumpTicketDb,
			" Generate Xorpads", &PadGen,
			" NAND Options", &NandMenu,
		},
		5,
		0,
		0
	};

#endif