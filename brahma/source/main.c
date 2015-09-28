#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "brahma.h"
#include "hid.h"
#include "menus.h"
#include "sochlp.h"
#include "payload_bin.h"

s32 quick_boot_firm (s32 load_from_disk) {
	if (load_from_disk)
		load_arm9_payload_from_mem(payload_bin, payload_bin_size);
	firm_reboot();	
}

s32 main(void) {
	// Initialize services
	gfxInitDefault();
	gfxSet3D(true);
	gfxSwapBuffers();

	Handle fileHandle;
	u32 bytesRead;
	FS_archive sdmcArchive = (FS_archive){ ARCH_SDMC, (FS_path){ PATH_EMPTY, 1, (u8*)"" } };
	FS_path filePath = FS_makePath(PATH_CHAR, "/" CODE_PATH);
	Result ret = FSUSER_OpenFileDirectly(NULL, &fileHandle, sdmcArchive, filePath, FS_OPEN_READ, FS_ATTRIBUTE_NONE);
	if (ret) goto EXIT;
	FSFILE_Read(fileHandle, &bytesRead, 0x14000, 0x14400000, 320 * 1024);
	FSFILE_Close(fileHandle);


	if (brahma_init())
	{
		quick_boot_firm(1);
		brahma_exit();
	}

EXIT:
	gfxExit();
	// Return to hbmenu
	return 0;
}
