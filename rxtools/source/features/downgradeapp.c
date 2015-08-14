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

#include <inttypes.h>
#include <stdbool.h>
#include "downgradeapp.h"
#include "screenshot.h"
#include "fs.h"
#include "fatfs/ff.h"
#include "console.h"
#include "draw.h"
#include "lang.h"
#include "hid.h"
#include "ncch.h"
#include "crypto.h"
#include "TitleKeyDecrypt.h"
#include "NandDumper.h"
#include "polarssl/aes.h"
#include "polarssl/sha2.h"
#include "stdio.h"

#define bswap_16(a) ((((a) << 8) & 0xff00) | (((a) >> 8) & 0xff))
#define bswap_32(a) ((((a) << 24) & 0xff000000) | (((a) << 8) & 0xff0000) | (((a) >> 8) & 0xff00) | (((a) >> 24) & 0xff))

typedef struct {
	unsigned int id;
	unsigned short index;
	unsigned short type;
	unsigned int size;
	unsigned char signature[0x20];
} tmd_chunk_struct;

unsigned char region = 0;

char tmpstr[256];
FILINFO curInfo;
DIR myDir;

void sprint_sha256(wchar_t *str, unsigned char hash[32])
{
	int i;
	for (i = 0; i < sizeof(hash); i++)
	{
		if ( (i & 0x10) == 0){
			swprintf(str, 2, L"\n");
			str++;
		}
		swprintf(str, 3, L"%02X", hash[i]);
		str+=2;
	}
}


int FindApp(AppInfo *info)
{
	char *folder = (char*)&tmpstr;
	memset(folder, 0, 256);

	DIR* curDir = &myDir;
	memset((unsigned char*)curDir, 0, sizeof(DIR));

	FILINFO *myInfo = &curInfo;
	memset((unsigned char*)myInfo, 0, sizeof(FILINFO));
	myInfo->fname[0] = 'A';

	sprintf(folder, "%d:title/%08" PRIx32 "/%08" PRIx32 "/content",
		info->drive, info->tidLo, info->tidHi);

	if (f_opendir(curDir, folder) != FR_OK) return 0;

	char path[256];
	unsigned short latest_ver = 0, cur_ver = 0;
	bool is_v0 = false;

	for (int i = 0; myInfo->fname[0] != 0; i++)
	{
		if (f_readdir(curDir, myInfo)) break;
		if (myInfo->fname[0] == '.') continue;

		if (strstr(myInfo->fname, ".tmd") || strstr(myInfo->fname, ".TMD"))
		{
			memset(&path, 0, 256);
			sprintf(path, "%s/%s", folder, myInfo->fname);

			File tmp;
			if (!FileOpen(&tmp, path, 0)) continue;

			unsigned int size = FileGetSize(&tmp);
			if (size < 0xB34)
			{
				FileClose(&tmp);
				continue;
			}

			/* Get the TMD version */
			/* There can be some instances in which more than one TMD/content file is available */
			/* Of course, we want to use the latest one */
			if (FileRead(&tmp, &cur_ver, 2, 0x1DC) != 2)
			{
				FileClose(&tmp);
				continue;
			}

			/* Change Endianness */
			cur_ver = bswap_16(cur_ver);

			/* Verify the version number */
			if ((latest_ver == 0 && !is_v0) || cur_ver > latest_ver)
			{
				tmd_chunk_struct tmd_entry;
				memset(&tmd_entry, 0xFF, 0x30);

				int cont = 0;
				unsigned int b_read = 0;
				while (tmd_entry.index != 0)
				{
					cont++;
					b_read = FileRead(&tmp, &tmd_entry, 0x30, size - (cont * 0x30));
					if (b_read != 0x30) break;
				}

				FileClose(&tmp);

				if (b_read != 0x30) continue;

				memset(&path, 0, 256);
				sprintf(path, "%s/%08x.app", folder, bswap_32(tmd_entry.id)); // Change Endianness

				if (FileOpen(&tmp, path, 0))
				{
					FileClose(&tmp);
					latest_ver = cur_ver;
					if (cur_ver == 0) is_v0 = true;

					/* Save TMD and content paths */
					sprintf(info->tmd, "%s/%s", folder, myInfo->fname);
					sprintf(info->content, "%s/%08x.app", folder, bswap_32(tmd_entry.id));
				}
			} else {
				FileClose(&tmp);
			}
		} else {
			continue;
		}
	}

	f_closedir(curDir);
	if (latest_ver == 0 && !is_v0) return 0;
	return 1;
}

int CheckRegion(int drive)
{
	File secureinfo;
	const char *filename="SecureInfo_A";
	sprintf(tmpstr, "%d:rw/sys/%s", drive, filename);
	print(strings[STR_OPENING], filename);
	ConsoleShow();
	if (!FileOpen(&secureinfo, tmpstr, 0))
	{
		filename="SecureInfo_B";
		sprintf(tmpstr, "%d:rw/sys/SecureInfo_B", drive);
		print(strings[STR_FAILED]);
		print(strings[STR_OPENING], filename);
		ConsoleShow();
		if (!FileOpen(&secureinfo, tmpstr, 0))
		{
			print(strings[STR_FAILED]);
			ConsoleShow();
			return -1;
		}
	}

	print(strings[STR_COMPLETED]);
	ConsoleShow();
	FileRead(&secureinfo, &region, 1, 0x100);
	FileClose(&secureinfo);

	if (region > 0x06)
	{
		print(strings[STR_WRONG], "", strings[STR_REGION]);
		ConsoleShow();
		return -1;
	} else {
		/* Avoid problems with the unused "AUS" region code */
		if (region >= 3) region--;
		print(strings[STR_REGION_], strings[STR_JAPAN+region]);
		ConsoleShow();
	}

	return 0;
}

int CheckRegionSilent(int drive)
{
	File secureinfo;
	sprintf(tmpstr, "%d:rw/sys/SecureInfo_A", drive);
	if (!FileOpen(&secureinfo, tmpstr, 0))
	{
		memset(&tmpstr, 0, 256);
		sprintf(tmpstr, "%d:rw/sys/SecureInfo_B", drive);
		if (!FileOpen(&secureinfo, tmpstr, 0))
		{
			print(strings[STR_ERROR_OPENING], tmpstr);
			ConsoleShow();
			return -1;
		}
	}

	FileRead(&secureinfo, &region, 1, 0x100);
	FileClose(&secureinfo);

	if (region > 0x06)
	{
		print(strings[STR_WRONG], "", strings[STR_REGION]);
		ConsoleShow();
		return -1;
	} else {
		/* Avoid problems with the unused "AUS" region code */
		if (region >= 3) region--;
	}

	return 0;
}

static unsigned int HashGen(unsigned char* file, unsigned int size)
{
	unsigned tbl[256];
	unsigned crc;
	for (unsigned i = 0; i < 256; i++)
	{
		crc = i << 24;
		for (unsigned j = 8; j > 0; j--)
		{
			if (crc & 0x80000000)
				crc = (crc << 1) ^ 0x04c11db7;
			else
				crc = (crc << 1);
			tbl[i] = crc;
		}
	}
	crc = 0;
	for (unsigned i = 0; i < size; i++)
		crc = (crc << 8) ^ tbl[((crc >> 24) ^ *file++) & 0xFF];
	for (; size; size >>= 8)
		crc = (crc << 8) ^ tbl[((crc >> 24) ^ size) & 0xFF];
	return ~crc;
}

int checkDgFile(char* path, unsigned int hash)
{
	unsigned char* buf = (unsigned char*)0x21000000;
	unsigned int rb, fixedsize = 0x00400000;

	File fp;
	if (FileOpen(&fp, path, 0))
	{
		rb = FileRead(&fp, buf, fixedsize, 0);
		FileClose(&fp);
		if (HashGen(buf, rb) == hash) return 0;
	} else {
		return 0;
	}

	return 1;
}

void downgradeMSET()
{
	File dg;
	char *dgpath = "0:msetdg.bin";
	unsigned int titleid_high[6] = { 0x00020000, 0x00021000, 0x00022000, 0x00026000, 0x00027000, 0x00028000 }; //JPN, USA, EUR, CHN, KOR, TWN
	unsigned int mset_hash[10] = { 0x96AEC379, 0xED315608, 0x3387F2CD, 0xEDAC05D7, 0xACC1BE62, 0xF0FF9F08, 0x565BCF20, 0xA04654C6, 0x2164C3C0, 0xD40B12F4 }; //JPN, USA, EUR, CHN, KOR, TWN
	unsigned short mset_ver[10] = { 3074, 5127, 3078, 5128, 3075, 5127, 8, 1026, 2049, 8 };
	unsigned short mset_dg_ver = 0;
	unsigned int checkLoop = 0;
	AppInfo info;

	ConsoleInit();
	ConsoleSetTitle(strings[STR_DOWNGRADE], strings[STR_MSET]);

	CheckRegionSilent(SYS_NAND);

	print(strings[STR_CHOOSE], strings[STR_MSET]);
	print(strings[STR_BLANK_BUTTON_ACTION], strings[STR_BUTTON_X], strings[STR_MSET4]);
	if( region != 3 && region != 5 )
	{
		print(strings[STR_BLANK_BUTTON_ACTION], strings[STR_BUTTON_Y], strings[STR_MSET6]);
	}
	print(strings[STR_BLANK_BUTTON_ACTION], strings[STR_BUTTON_B], strings[STR_CANCEL]);
	ConsoleShow();

	while( checkLoop < 1 )
	{
		uint32_t pad_state = InputWait();
		if ((pad_state & BUTTON_X))
		{
			switch(region){
				case 0:
					mset_dg_ver = 0;
					break;
				case 1:
					mset_dg_ver = 2;
					break;
				case 2:
					mset_dg_ver = 4;
					break;
				case 3:
					mset_dg_ver = 6;
					break;
				case 4:
					mset_dg_ver = 7;
					break;
				case 5:
					mset_dg_ver = 9;
					break;
			}
			checkLoop = 1;
		}
		else if ((pad_state & BUTTON_Y)  && region != 3 && region != 5)
		{
			switch(region){
				case 0:
					mset_dg_ver = 1;
					break;
				case 1:
					mset_dg_ver = 3;
					break;
				case 2:
					mset_dg_ver = 5;
					break;
				case 4:
					mset_dg_ver = 8;
					break;
			}
			checkLoop = 1;
		}
		else if (pad_state & BUTTON_B)
		{
			checkLoop = 1;
			return;
		}
	}
	if( mset_dg_ver == 0 )
	{
		print(strings[STR_WRONG], "", strings[STR_REGION]);
	}
	ConsoleShow();

	print(strings[STR_PROCESSING], strings[STR_MSET]);
	ConsoleShow();

	if (CheckRegion(SYS_NAND) == 0)
	{
		info.drive = SYS_NAND;
		info.tidLo = 0x00040010;
		info.tidHi = titleid_high[region];
		if (FindApp(&info)) // SysNAND only
		{
			if (FileOpen(&dg, info.tmd, 0))
			{
				/* Get the MSET TMD version */
				unsigned short tmd_ver;
				FileRead(&dg, &tmd_ver, 2, 0x1DC);
				tmd_ver = bswap_16(tmd_ver);
				FileClose(&dg);

				/* Verify version number */
				if (tmd_ver != mset_ver[mset_dg_ver])
				{
					/* Open MSET content file */
					if (FileOpen(&dg, info.content, 0))
					{
						unsigned int check_val;
						FileRead(&dg, &check_val, 4, 0x130);
						FileClose(&dg);

						if (check_val != 0)
						{
							if (checkDgFile(dgpath, mset_hash[mset_dg_ver]))
							{
								print(strings[STR_PROCESSING], strings[STR_DOWNGRADE_PACK]);
								ConsoleShow();
								if (FileOpen(&dg, dgpath, 0))
								{
									unsigned int dgsize = FileGetSize(&dg);
									unsigned char *buf = (unsigned char*)0x21000000;
									FileRead(&dg, buf, dgsize, 0);

									/* Downgrade pack decryption */
									uint8_t iv[0x10] = {0};
									uint8_t Key[0x10] = {0};

									GetTitleKey(&Key[0], info.tidLo, info.tidHi, info.drive);

									aes_context aes_ctxt;
									aes_setkey_dec(&aes_ctxt, Key, 0x80);
									aes_crypt_cbc(&aes_ctxt, AES_DECRYPT, dgsize, iv, buf, buf);

									FileWrite(&dg, buf, dgsize, 0);
									FileClose(&dg);

									if (*((unsigned int*)(buf + 0x100)) == 0x4843434E) // "NCCH" magic word
									{
										print(strings[STR_DOWNGRADING], L"");
										ConsoleShow();
										if (FSFileCopy(info.content, dgpath) != 0)
										{
											print(strings[STR_FAILED]);
										}
										print(strings[STR_DELETING], dgpath);
										ConsoleShow();
										f_unlink(dgpath);
									} else {
										print(strings[STR_WRONG], "", strings[STR_DOWNGRADE_PACK]);
									}
								} else {
									print(strings[STR_ERROR_OPENING], dgpath);
								}
							} else {
								print(strings[STR_WRONG], "", strings[STR_DOWNGRADE_PACK]);
							}
						} else {
							print(strings[STR_DOWNGRADING_NOT_NEEDED], strings[STR_MSET]);
						}
					} else {
						print(strings[STR_ERROR_OPENING], info.content);
					}
				} else {
					print(strings[STR_DOWNGRADING_NOT_NEEDED], strings[STR_MSET]);
				}
			} else {
				print(strings[STR_ERROR_OPENING], info.tmd);
			}
		} else {
			print(strings[STR_MISSING], strings[STR_MSET]);
		}
	}

	print(strings[STR_BLANK_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

void manageFBI(bool restore)
{
	unsigned int titleid_high[6] = { 0x00020300, 0x00021300, 0x00022300, 0x00026300, 0x00027300, 0x00028300 }; //JPN, USA, EUR, CHN, KOR, TWN
	char *backup_path = "rxTools/h&s_backup";

	File tmp;
	char path[256] = {0};
	char path2[256] = {0};
	wchar_t wtmp[67];
	unsigned char *buf = (unsigned char *)0x21000000;

	unsigned int size;
	unsigned short tmd_ver;
	unsigned int sd_cntsize;
	unsigned short sd_tmd_ver;

	unsigned char TmdCntInfoRecSum[32] = {0};
	unsigned char CntInfoRecSum[32] = {0};
	unsigned char TmdCntChnkRecSum[32] = {0};
	unsigned char CntChnkRecSum[32] = {0};
	unsigned char TmdCntDataSum[32] = {0};
	unsigned char CntDataSum[32] = {0};

	unsigned short checkLoop;
	bool noHalt = true;

	AppInfo info;

	info.drive = NandSwitch();
	if (info.drive == UNK_NAND)
		return;

	info.tidLo = 0x00040010;

	ConsoleInit();
	if (restore)
	{
		ConsoleSetTitle(strings[STR_RESTORE], strings[STR_HEALTH_AND_SAFETY]);
		ConsoleShow();
	}
	else
	{
		ConsoleSetTitle(strings[STR_INJECT], strings[STR_FBI]);
		print(strings[STR_CHOOSE], "");
		print(strings[STR_BLANK_BUTTON_ACTION], strings[STR_BUTTON_B], strings[STR_CHECK_TMD_ONLY]);
		print(strings[STR_BLANK_BUTTON_ACTION], strings[STR_BUTTON_Y], strings[STR_INJECT_FBI]);
		ConsoleShow();
		checkLoop = 0;

		while(checkLoop < 1)
		{
			uint32_t pad_state = InputWait();
			if (pad_state & BUTTON_Y)
			{
				noHalt = true;
				checkLoop = 1;
			}
			else if (pad_state & BUTTON_B)
			{
				noHalt = false;
				checkLoop = 1;

				CheckRegion(info.drive);

				info.tidHi = titleid_high[region];
				if (FindApp(&info))
				{
					/* Open the NAND H&S TMD */
					FileOpen(&tmp, info.tmd, 0);
					FileRead(&tmp, buf, 0xB34, 0);
					FileClose(&tmp);

					/* Get the title version from the TMD */
					tmd_ver = (unsigned short)((buf[0x1DC] << 8) | buf[0x1DD]);
					print(strings[STR_VERSION_OF], strings[STR_TMD], tmd_ver);
				}
			}
		}
	}

	if(noHalt)
	{

		if (CheckRegion(info.drive) == 0)
		{
			info.tidHi = titleid_high[region];
			if (FindApp(&info))
			{
				/* Open the NAND H&S TMD */
				FileOpen(&tmp, info.tmd, 0);
				FileRead(&tmp, buf, 0xB34, 0);
				FileClose(&tmp);

				/* Get the title version from the TMD */
				tmd_ver = (unsigned short)((buf[0x1DC] << 8) | buf[0x1DD]);
				print(strings[STR_VERSION_OF], strings[STR_TMD], tmd_ver);
				ConsoleShow();

				if (!restore)
				{
					/* Get the stored content size from the TMD */
					unsigned int cntsize = (unsigned int)((buf[0xB10] << 24) | (buf[0xB11] << 16) | (buf[0xB12] << 8) | buf[0xB13]);

					/* Open the NAND H&S content file and read it to the memory buffer */
					FileOpen(&tmp, info.content, 0);
					FileRead(&tmp, buf + 0x1000, cntsize, 0);
					FileClose(&tmp);

					/* Create the Health & Safety data backup directory */
					f_mkdir(backup_path);

					memset(&tmpstr, 0, 256);
					sprintf(tmpstr, "%s/%ls", backup_path, strings[STR_JAPAN+region]);
					f_mkdir(tmpstr);

					memset(&tmpstr, 0, 256);
					sprintf(tmpstr, "%s/%ls/v%u", backup_path, strings[STR_JAPAN+region], tmd_ver);
					f_mkdir(tmpstr);

					/* Backup the H&S TMD */
					print(strings[STR_BACKING_UP], strings[STR_HEALTH_AND_SAFETY]);
					ConsoleShow();
					sprintf(path, "0:%s/%.12s", tmpstr, info.tmd+34);
					if (FileOpen(&tmp, path, 1))
					{
						size = FileWrite(&tmp, buf, 0xB34, 0);
						FileClose(&tmp);
						if (size == 0xB34)
						{
							/* Backup the H&S content file */
							memset(&path, 0, 256);
							sprintf(path, "0:%s/%.12s", tmpstr, info.content+34);
							if (FileOpen(&tmp, path, 1))
							{
								size = FileWrite(&tmp, buf + 0x1000, cntsize, 0);
								FileClose(&tmp);
								if (size != cntsize)
								{
									print(strings[STR_ERROR_WRITING], path);
									goto out;
								}
							} else {
								print(strings[STR_ERROR_CREATING], path);
								goto out;
							}
						} else {
							print(strings[STR_ERROR_WRITING], path);
							goto out;
						}
					} else {
						print(strings[STR_ERROR_CREATING], path);
						goto out;
					}

					/* Generate the FBI data paths */
					sprintf(path, "0:fbi_inject.tmd");
					sprintf(path2, "0:fbi_inject.app");

					print(strings[STR_INJECTING], "", strings[STR_FBI]);
				} else {
					/* Generate the H&S backup data paths */
					memset(&tmpstr, 0, 256);
					sprintf(tmpstr, "%s/%ls/v%u", backup_path, strings[STR_JAPAN+region], tmd_ver);
					sprintf(path, "0:%s/%.12s", tmpstr, info.tmd+34);
					sprintf(path2, "0:%s/%.12s", tmpstr, info.content+34);

					print(strings[STR_RESTORING], strings[STR_HEALTH_AND_SAFETY]);
				}
				ConsoleShow();

				/* Open the SD TMD */
				if (FileOpen(&tmp, path, 0))
				{
					size = FileGetSize(&tmp);
					if (size == 0xB34)
					{
						FileRead(&tmp, buf, 0xB34, 0);
						FileClose(&tmp);

						/* Get the SD TMD version and stored content size */
						sd_tmd_ver = (unsigned short)((buf[0x1DC] << 8) | buf[0x1DD]);
						sd_cntsize = (unsigned int)((buf[0xB10] << 24) | (buf[0xB11] << 16) | (buf[0xB12] << 8) | buf[0xB13]);

						if (sd_tmd_ver == tmd_ver)
						{
							/* Get the SHA-256 hashes */
							memcpy(TmdCntInfoRecSum, buf + 0x1E4, 32);
							memcpy(TmdCntChnkRecSum, buf + 0x208, 32);
							memcpy(TmdCntDataSum, buf + 0xB14, 32);

							/* Verify the Content Info Record hash */
							sha2(buf + 0x204, 0x900, CntInfoRecSum, 0);
							if (memcmp(CntInfoRecSum, TmdCntInfoRecSum, 32) == 0)
							{
								/* Verify the Content Chunk Record hash */
								sha2(buf + 0xB04, 0x30, CntChnkRecSum, 0);
								if (memcmp(CntChnkRecSum, TmdCntChnkRecSum, 32) == 0)
								{
									/* Open the SD content file */
									if (FileOpen(&tmp, path2, 0))
									{
										size = FileGetSize(&tmp);
										if (size == sd_cntsize)
										{
											FileRead(&tmp, buf + 0x1000, sd_cntsize, 0);
											FileClose(&tmp);

											/* Verify the Content Data hash */
											sha2(buf + 0x1000, sd_cntsize, CntDataSum, 0);
											if (memcmp(CntDataSum, TmdCntDataSum, 32) == 0)
											{
												/* Now we are ready to rock 'n roll */
												if (FSFileCopy(info.tmd, path) == 0)
												{
													if (FSFileCopy(info.content, path2) == 0)
													{
														print(strings[STR_CHOOSE], strings[STR_SOURCE_ACTION]);
														print(strings[STR_BLANK_BUTTON_ACTION], strings[STR_BUTTON_B], strings[STR_KEEP]);
														print(strings[STR_BLANK_BUTTON_ACTION], strings[STR_BUTTON_X], strings[STR_DELETE]);
														ConsoleShow();
														checkLoop = 0;

														while (checkLoop < 1)
														{
															uint32_t pad_state = InputWait();
															if (pad_state & BUTTON_B)
															{
																checkLoop = 1;
															}
															else if (pad_state & BUTTON_X)
															{
																print(strings[STR_DELETING], path);
																ConsoleShow();
																f_unlink(path);
																print(strings[STR_DELETING], path2);
																ConsoleShow();
																f_unlink(path2);
																checkLoop = 1;
															}
														}
													} else {
														print(strings[STR_ERROR_COPYING], path2, info.content);
													}
												} else {
													print(strings[STR_ERROR_COPYING], path, info.tmd);
												}
											} else {
												print(strings[STR_WRONG], "", strings[STR_HASH]);
												sprint_sha256(wtmp, CntDataSum);
												print(strings[STR_GOT], wtmp);
												sprint_sha256(wtmp, TmdCntDataSum);
												print(strings[STR_EXPECTED], wtmp);
											}
										} else {
											FileClose(&tmp);
											print(strings[STR_WRONG], "", strings[STR_SIZE]);
											swprintf(wtmp, sizeof(wtmp)/sizeof(wtmp[0]), L"%u", size);
											print(strings[STR_GOT], wtmp);
											swprintf(wtmp, sizeof(wtmp)/sizeof(wtmp[0]), L"%u", sd_cntsize);
											print(strings[STR_EXPECTED], wtmp);
										}
									} else {
										print(strings[STR_ERROR_OPENING], path2);
									}
								} else {
									print(strings[STR_WRONG], "", strings[STR_HASH]);
									sprint_sha256(wtmp, CntChnkRecSum);
									print(strings[STR_GOT], wtmp);
									sprint_sha256(wtmp, TmdCntChnkRecSum);
									print(strings[STR_EXPECTED], wtmp);
								}
							} else {
								print(strings[STR_WRONG], "", strings[STR_HASH]);
								sprint_sha256(wtmp, CntInfoRecSum);
								print(strings[STR_GOT], wtmp);
								sprint_sha256(wtmp, TmdCntInfoRecSum);
								print(strings[STR_EXPECTED], wtmp);
							}
						} else {
							print(strings[STR_WRONG], "", strings[STR_TMD_VERSION]);
						}
					} else {
						FileClose(&tmp);
						print(strings[STR_WRONG], "", strings[STR_TMD_SIZE]);
					}
				} else {
					print(strings[STR_ERROR_OPENING], path);
				}
			} else {
				print(strings[STR_MISSING], strings[STR_HEALTH_AND_SAFETY]);
			}
		}
	}
out:
	print(strings[STR_BLANK_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

void installFBI()
{
	/* Injects FBI TMD and content file to the Health & Safety App */
	manageFBI(false);
}

void restoreHS()
{
	/* Restores original Health & Safety TMD and content file to the NAND */
	manageFBI(true);
}
