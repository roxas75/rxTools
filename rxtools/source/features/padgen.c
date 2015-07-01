#include <string.h>

#include "fs.h"
#include "draw.h"
#include "padgen.h"
#include "crypto.h"
#include "console.h"
#include "hid.h"

char STR[256];

void PadGen(){
	ConsoleInit();
	ConsoleSetTitle("Xorpad Generator");
	NcchPadgen(); ConsoleShow();
	SdPadgen(); ConsoleShow();
	
	print("\nPress A to exit\n"); ConsoleShow();
	WaitForButton(BUTTON_A);
}

u32 NcchPadgen()
{
//    size_t bytesRead;
    u32 result;
	File pf;
    NcchInfo *info = (NcchInfo*)0x20316000;

    if (!FileOpen(&pf, "/ncchinfo.bin", 0)) {
        print("Could not open ncchinfo.bin!\n");
        return 1;
    }
/*    bytesRead = */FileRead(&pf, info, 16, 0);

    if (!info->n_entries || info->n_entries > MAXENTRIES || (info->ncch_info_version != 0xF0000003)) {
        print("Too many/few entries, or \nwrong version ncchinfo.bin!\n");
        return 0;
    }
/*    bytesRead = */FileRead(&pf, info->entries, info->n_entries * sizeof(NcchInfoEntry), 16);
    FileClose(&pf);
	
	print("Working on ncchinfo.bin ...\n"); ConsoleShow();
    for(u32 i = 0; i < info->n_entries; i++) {		

        PadInfo padInfo = {.setKeyY = 1, .size_mb = info->entries[i].size_mb};
        memcpy(padInfo.CTR, info->entries[i].CTR, 16);
        memcpy(padInfo.keyY, info->entries[i].keyY, 16);
        memcpy(padInfo.filename, info->entries[i].filename, 112);

        if(info->entries[i].uses7xCrypto)
            padInfo.keyslot = 0x25;
        else
            padInfo.keyslot = 0x2C;

        result = CreatePad(&padInfo, i);
        if (result) return 1;
    }

    return 0;
}

u32 SdPadgen()
{
    size_t bytesRead;
    u32 result;
    File fp;
    SdInfo *info = (SdInfo*)0x20316000;

    u8 movable_seed[0x120] = {0};

    // Load console 0x34 keyY from movable.sed if present on SD card
    if (FileOpen(&fp, "/movable.sed", 0)) {
        bytesRead = FileRead(&fp, &movable_seed, 0x120, 0);
        FileClose(&fp);
        if (bytesRead != 0x120) {
            print("movable.sed is too small!\n");
            return 1;
        }
        if (memcmp(movable_seed, "SEED", 4) != 0) {
            print("movable.sed is corrupted!\n");
            return 1;
        }
		print("movable.sed loaded!\n");
        setup_aeskey(0x34, AES_BIG_INPUT|AES_NORMAL_INPUT, &movable_seed[0x110]);
        use_aeskey(0x34);
    }

    if (!FileOpen(&fp, "/SDinfo.bin", 0)) {
        print("Could not open SDinfo.bin!\n");
        return 1;
    }
    bytesRead = FileRead(&fp, info, 4, 0);

    if (!info->n_entries || info->n_entries > MAXENTRIES) {
        print("Too many/few entries!\n");
        return 1;
    }

    print("Working on SDinfo.bin ...\n"); ConsoleShow();

    bytesRead = FileRead(&fp, info->entries, info->n_entries * sizeof(SdInfoEntry), 4);
    FileClose(&fp);

    for(u32 i = 0; i < info->n_entries; i++) {
        PadInfo padInfo = {.keyslot = 0x34, .setKeyY = 0, .size_mb = info->entries[i].size_mb};
        memcpy(padInfo.CTR, info->entries[i].CTR, 16);
        memcpy(padInfo.filename, info->entries[i].filename, 180);

        result = CreatePad(&padInfo, i);
        if (!result){
		
        }else
            return 1;
    }

    return 0;
}

static const uint8_t zero_buf[16] __attribute__((aligned(16))) = {0};

u32 CreatePad(PadInfo *info, int index)
{
File pf;
#define BUFFER_ADDR ((volatile uint8_t*)0x21000000)
#define BLOCK_SIZE  (4*1024*1024)
//    size_t bytesWritten;

    if (!FileOpen(&pf, info->filename, 1))
        return 1;

    if(info->setKeyY != 0)
        setup_aeskey(info->keyslot, AES_BIG_INPUT|AES_NORMAL_INPUT, info->keyY);
    use_aeskey(info->keyslot);

    u8 ctr[16] __attribute__((aligned(32)));
    memcpy(ctr, info->CTR, 16);

    u32 size_bytes = info->size_mb*1024*1024;
    u32 size_100 = size_bytes/100;
    u32 seekpos = 0;
    for (u32 i = 0; i < size_bytes; i += BLOCK_SIZE) {
        u32 j;
        for (j = 0; (j < BLOCK_SIZE) && (i+j < size_bytes); j+= 16) {
            set_ctr(AES_BIG_INPUT|AES_NORMAL_INPUT, ctr);
            aes_decrypt((void*)zero_buf, (void*)BUFFER_ADDR+j, ctr, 1, AES_CTR_MODE);
            add_ctr(ctr, 1);
        }
		
		print("Creating Pad %i : %i%%      ", index, (i+j)/size_100);
		ConsolePrevLine(); ConsolePrevLine(); ConsoleShow();
/*		bytesWritten = */FileWrite(&pf, (void*)BUFFER_ADDR, j, seekpos);
        seekpos += j;
    }

    FileClose(&pf);
    return 0;
}
