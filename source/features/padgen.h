#ifndef PADGEN_H
#define PADGEN_H

#include "common.h"

#define MAXENTRIES 1024

typedef struct {
    u8   CTR[16];
    u32  size_mb;
    char filename[180];
} __attribute__((packed)) SdInfoEntry;

typedef struct {
    u32 n_entries;
    SdInfoEntry entries[MAXENTRIES];
} __attribute__((packed, aligned(16))) SdInfo;


typedef struct {
    u8   CTR[16];
    u8   keyY[16];
    u32  size_mb;
    u8   reserved[8];
    u32  uses7xCrypto;
    char filename[112];
} __attribute__((packed)) NcchInfoEntry;

typedef struct {
    u32 padding;
    u32 ncch_info_version;
    u32 n_entries;
    u8  reserved[4];
    NcchInfoEntry entries[MAXENTRIES];
} __attribute__((packed, aligned(16))) NcchInfo;


typedef struct {
    u32  keyslot;
    u32  setKeyY;
    u8   CTR[16];
    u8   keyY[16];
    u32  size_mb;
    char filename[180];
} __attribute__((packed, aligned(16))) PadInfo;

u32 CreatePad(PadInfo *info, int index);
u32 NcchPadgen();
u32 SdPadgen();
void PadGen();

#endif
