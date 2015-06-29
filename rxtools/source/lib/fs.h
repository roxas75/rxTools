#pragma once

#include "common.h"
#include "ff.h"
#include "nand.h"
#define File FIL

////////////////////////////////////////////////////////////////Basic FileSystem Operations
bool FSInit(void);
void FSDeInit(void);
bool FileOpen(File *Handle, const char *path, bool truncate);
size_t FileRead(File *Handle, void *buf, size_t size, size_t foffset);
size_t FileWrite(File *Handle, void *buf, size_t size, size_t foffset);
size_t FileGetSize(File *Handle);
void FileClose(File *Handle);
////////////////////////////////////////////////////////////////Advanced FileSystem Operations
u32 FSFileCopy(char *target, char *source);
