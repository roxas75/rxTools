/*
 * Copyright (C) 2015 The PASTA Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "fs.h"
#include "fatfs/ff.h"

////////////////////////////////////////////////////////////////Basic FileSystem Operations
static FATFS fs[3];
/**Init FileSystems.*/
bool FSInit(void) {
	FSNandInitCrypto();
	if (f_mount(&fs[0], "0:", 0) != FR_OK) return 0;		//SDCard
	if (f_mount(&fs[1], "1:", 0) != FR_OK) return 0;		//NAND
	if (f_mount(&fs[2], "2:", 0) != FR_OK) ; //return 0;	//EmuNAND, Sometimes it doesn't exist
	return 1;
}
/**[Unused?]DeInit FileSystems.*/
void FSDeInit(void) {
	f_mount(NULL, "0:", 0);
	f_mount(NULL, "1:", 0);
	f_mount(NULL, "2:", 0);
}

bool FileOpen(File *Handle, const char *path, bool truncate) {
	unsigned flags = FA_READ | FA_WRITE;
	flags |= truncate ? FA_CREATE_ALWAYS : FA_OPEN_EXISTING; //: FA_OPEN_ALWAYS;
	bool ret = (f_open(Handle, path, flags) == FR_OK);
	if (f_tell(Handle)) { f_lseek(Handle, 0); } //Only seek to head if not
	f_sync(Handle);
	return ret;
}

size_t FileRead(File *Handle, void *buf, size_t size, size_t foffset) {
	UINT bytes_read = 0;
	if (f_tell(Handle) != foffset) { f_lseek(Handle, foffset); } //Avoid crazy lseeks
	f_read(Handle, buf, size, &bytes_read);
	return bytes_read;
}

size_t FileWrite(File *Handle, void *buf, size_t size, size_t foffset) {
	UINT bytes_written = 0;
	if (f_tell(Handle) != foffset) { f_lseek(Handle, foffset); } //Avoid crazy lseeks
	f_write(Handle, buf, size, &bytes_written);
	f_sync(Handle);
	return bytes_written;
}

size_t FileGetSize(File *Handle) {
	return f_size(Handle);
}

void FileClose(File *Handle) {
	f_close(Handle);
}
////////////////////////////////////////////////////////////////Advanced FileSystem Operations
/** Copy Source File (source) to Target (target).
  * @param  target Target file, will be created if not exists.
  * @param  source Source file, must exists.
  * @retval Compounded value of (STEP<<8)|FR_xx, so it contains real reasons.
  * @note   directly FATFS calls. FATFS return value only ranges from 0 to 19.
  */
uint32_t FSFileCopy(char *target, char *source) {
	FIL src, dst;
	uint32_t step = 0; //Tells you where it failed
	FRESULT retfatfs = 0; //FATFS return value
	uint32_t blockSize = 0x4000;
	uint8_t *buffer = (uint8_t *)0x26000200; //Temp buffer for transfer.
	UINT byteI = 0, byteO = 0; //Bytes that read or written
	retfatfs = f_open(&src, source, FA_READ | FA_OPEN_EXISTING);
	if (retfatfs != FR_OK) {step = 1; goto closeExit;}
	retfatfs = f_open(&dst, target, FA_WRITE | FA_CREATE_ALWAYS);
	if (retfatfs != FR_OK) {step = 2; goto closeExit;}
	uint32_t totalSize = src.fsize;
	//If source file has no contents, nothing to be copied.
	if (!totalSize) { goto closeExit; }
	while (totalSize) {
		if (totalSize < blockSize) { blockSize = totalSize; }
		//FR_OK, FR_DISK_ERR, FR_INT_ERR, FR_INVALID_OBJECT, FR_TIMEOUT
		retfatfs = f_read(&src, buffer, blockSize, &byteI);
		if (retfatfs != FR_OK) {step = 3; goto closeExit;}
		//Unexpected
		if (byteI != blockSize) {step = 4; goto closeExit;}
		//FR_OK, FR_DISK_ERR, FR_INT_ERR, FR_INVALID_OBJECT, FR_TIMEOUT
		retfatfs = f_write(&dst, buffer, blockSize, &byteO);
		if (retfatfs != FR_OK) {step = 5; goto closeExit;}
		//Unexpected
		if (byteO != blockSize) {step = 6; goto closeExit;}
		totalSize -= blockSize;
	}
	step = 0; //Success return value.
closeExit:
	f_close(&src);
	f_close(&dst);
	return (step << 8) | retfatfs;
}
