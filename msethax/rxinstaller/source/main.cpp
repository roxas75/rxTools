#include <nds.h>
#include <stdio.h>
#include <fat.h>
#include <vector>
#include <string>

#include "drunkenlogo.h"
#include "mset4x_bin.h"
#include "mset6x_bin.h"

u8 workbuffer[1024] ALIGN(32);

#define SCREEN_COLS 32
#define ITEMS_PER_SCREEN 10
#define ITEMS_START_ROW 8
#define NUM_PATCHES 1

using namespace std;

typedef struct{
	char* name;
	const u8* patch;
} rop_patch;

rop_patch Patches[NUM_PATCHES] = {
	"mset loader 4.x", mset4x_bin,
	//"mset loader 5.x-9.x", mset6x_bin,
};

//---------------------------------------------------------------------------------
void halt() {
//---------------------------------------------------------------------------------
	int pressed;

	iprintf("\n  Press A to exit\n");

	while(1) {
		swiWaitForVBlank();
		scanKeys();
		pressed = keysDown();
		if (pressed & KEY_A) break;
	}
	exit(0);
}

//---------------------------------------------------------------------------------
void userSettingsCRC(void *buffer) {
//---------------------------------------------------------------------------------
	u16 *slot = (u16*)buffer;
	u16 CRC1 = swiCRC16(0xFFFF, slot, 0x70);
	u16 CRC2 = swiCRC16(0xFFFF, &slot[0x3a], 0x8A);
	slot[0x39] = CRC1; slot[0x7f] = CRC2;
}


//---------------------------------------------------------------------------------
void saveFile(char *name, void *buffer, int size) {
//---------------------------------------------------------------------------------
	FILE *out = fopen(name,"wb");
	if (out) {
		fwrite(buffer, 1, 1024, out);
		fclose(out);
	} else {
		printf("couldn't open %s for writing\n",name);
	}
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------
	videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    int bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
    dmaCopy(drunkenlogoBitmap, bgGetGfxPtr(bg), drunkenlogoBitmapLen); 
	consoleDemoInit();

	iprintf("\n\n");
	iprintf("  rxTools - MSET ROP Installer ");
	iprintf("\n\n\n");
	iprintf("  Select the exploit version :");

	int pressed,fwSelected=0,screenOffset=0;
	for(int i = 0; i < NUM_PATCHES; i++){
		iprintf ("\x1b[%d;6H", i + ITEMS_START_ROW);
		iprintf("%s", Patches[i].name);
	}
	while(1) {

		// Show cursor
		iprintf ("\x1b[%d;3H>\x1b[22C", fwSelected - screenOffset + ITEMS_START_ROW);

		// Power saving loop. Only poll the keys once per frame and sleep the CPU if there is nothing else to do
		do {
			scanKeys();
			pressed = keysDownRepeat();
			swiWaitForVBlank();
		} while (!pressed);

		// Hide cursor
		iprintf ("\x1b[%d;3H  \x1b[22C  ", fwSelected - screenOffset + ITEMS_START_ROW);
		if (pressed & KEY_UP) 		fwSelected -= 1;
		if (pressed & KEY_DOWN) 	fwSelected += 1;

		if (pressed & KEY_A) break;

		if (fwSelected < 0) 	fwSelected = NUM_PATCHES - 1;		// Wrap around to bottom of list
		if (fwSelected > NUM_PATCHES - 1)		fwSelected = 0;		// Wrap around to top of list


	}

	iprintf ("\x1b[5;0H\x1b[J");

	iprintf("  Installing: %s\n\n", Patches[fwSelected].name);

	// read header
	readFirmware(0,workbuffer,42);

	u32 userSettingsOffset = (workbuffer[32] + (workbuffer[33] << 8))<<3;

	// read User Settings
	readFirmware(userSettingsOffset,workbuffer,512);

	memcpy(workbuffer, Patches[fwSelected].patch, 0x200);

	userSettingsCRC(workbuffer);
	userSettingsCRC(workbuffer+256);


	iprintf("  Writing... ");
	int ret = writeFirmware(userSettingsOffset,workbuffer,512);

	if (ret) {

		iprintf("FAIL\n");

	} else {

		iprintf("OK\n");

	}

	iprintf("  Verifying... ");
	readFirmware(userSettingsOffset,workbuffer+512,512);

	if (memcmp(workbuffer,workbuffer+512,512)){

		iprintf("FAIL\n");

	} else {

		iprintf("OK\n");

	}

	halt();
	return 0;
}
