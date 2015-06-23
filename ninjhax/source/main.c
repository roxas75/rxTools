#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include "brahma.h"

void waitKey() {
	while (aptMainLoop()) {
		// Wait next screen refresh
		gspWaitForVBlank();

		// Read which buttons are currently pressed 
		hidScanInput();
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		
		// If B is pressed, break loop and quit
		if (kDown & KEY_B){
			break;
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
	}
}

int main() {
	// Initialize services
	srvInit();
	aptInit();
	hidInit(NULL);
	gfxInitDefault();
	fsInit();
	sdmcInit();
	hbInit();

	qtmInit();
	consoleInit(GFX_BOTTOM, NULL);
	
	Handle fileHandle;
	u32 bytesRead;
    FS_archive sdmcArchive=(FS_archive){ARCH_SDMC, (FS_path){PATH_EMPTY, 1, (u8*)""}};
    FS_path filePath=FS_makePath(PATH_CHAR, "/rxTools.dat");
    Result ret=FSUSER_OpenFileDirectly(NULL, &fileHandle, sdmcArchive, filePath, FS_OPEN_READ, FS_ATTRIBUTE_NONE);
	if(ret) goto EXIT;
    FSFILE_Read(fileHandle, &bytesRead, 0x20000, 0x14400000, 320*1024);
    FSFILE_Close(fileHandle);
	
	run_exploit();
		                     
	printf("\nPress [B] to return to launcher\n");
	waitKey();
	printf("[+] Exiting...\n");
	
	EXIT:
	// Exit services
	hbExit();
	sdmcExit();
	fsExit();
	gfxExit();
	hidExit();
	aptExit();
	srvExit();
	
	// Return to hbmenu
	return 0;
}
