#include "downgradeapp.h"
#include "common.h"
#include "screenshot.h"
#include "fs.h"
#include "ff.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "ncch.h"
#include "crypto.h"
#include "TitleKeyDecrypt.h"
#include "NandDumper.h"
#include "aes.h"
#include "polarssl/sha2.h"

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
char *regions[6] = { "Japan", "USA", "Europe", "China", "Korea", "Taiwan" };

char tmpstr[256];
FILINFO curInfo;
DIR myDir;

char cntpath[256]; // Contains the NAND content path
char tmdpath[256]; // Contains the NAND TMD path

void print_sha256(unsigned char hash[32])
{
	int i;
	for (i = 0; i < 32; i++)
	{
		print("%02X", hash[i]);
		
		if (i == 19)
		{
			/* Continue printing the SHA-256 sum in a new line after 40 characters (20 bytes) */
			print("\n"); ConsoleShow();
		}
	}
	
	ConsoleShow();
}

int FindApp(unsigned int tid_low, unsigned int tid_high, int drive)
{
    char *folder = &tmpstr;
    memset(folder, 0, 256);
	
    DIR* curDir = &myDir;
	memset((unsigned char*)curDir, 0, sizeof(DIR));
	
	FILINFO *myInfo = &curInfo;
    memset((unsigned char*)myInfo, 0, sizeof(FILINFO));
	myInfo->fname[0] = 'A';
	
    sprintf(folder, "%d:title/%08x/%08x/content", drive, tid_low, tid_high);
	
	if (f_opendir(curDir, folder) != FR_OK) return 0;
	
	unsigned short latest_ver = 0, cur_ver = 0;
	bool is_v0 = false;
	
	for (int i = 0; myInfo->fname[0] != 0; i++)
	{
		if (f_readdir(curDir, myInfo)) break;
		if (myInfo->fname[0] == '.') continue;
		
		if (strstr(myInfo->fname, ".tmd") || strstr(myInfo->fname, ".TMD"))
		{
			memset(&tmdpath, 0, 256);
			sprintf(tmdpath, "%s/%s", folder, myInfo->fname);
			
			File tmp;
			if (!FileOpen(&tmp, tmdpath, 0)) continue;
			
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
				
				memset(&cntpath, 0, 256);
				sprintf(cntpath, "%s/%08x.app", folder, bswap_32(tmd_entry.id)); // Change Endianness
				
				if (FileOpen(&tmp, cntpath, 0))
				{
					FileClose(&tmp);
					latest_ver = cur_ver;
					if (cur_ver == 0) is_v0 = true;
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
	print("Opening SecureInfo_A... "); ConsoleShow();
	sprintf(tmpstr, "%d:rw/sys/SecureInfo_A", drive);
	if (!FileOpen(&secureinfo, tmpstr, 0))
	{
		print("Error.\nTrying with SecureInfo_B... "); ConsoleShow();
		memset(&tmpstr, 0, 256);
		sprintf(tmpstr, "%d:rw/sys/SecureInfo_B", drive);
		if (!FileOpen(&secureinfo, tmpstr, 0))
		{
			print("Error.\nProcess failed!\n"); ConsoleShow();
			return -1;
		}
	}
	
	print("OK!\n"); ConsoleShow();
	FileRead(&secureinfo, &region, 1, 0x100);
	FileClose(&secureinfo);
	
	if (region > 0x06)
	{
		print("Error: unsupported region.\nProcess failed!\n"); ConsoleShow();
		return -1;
	} else {
		/* Avoid problems with the unused "AUS" region code */
		if (region >= 3) region--;
		print("Region: %s\n", regions[region]); ConsoleShow();
	}
	
	return 0;
}

int checkDgFile(char* path, unsigned int hash)
{
    unsigned char* buf = (unsigned char*)0x21000000;
    unsigned int rb, fixedsize = 0x00400000;
	
    File fp;
    if (FileOpen(&fp, path, 0))
	{
        rb = FileRead(&fp, buf, fixedsize, 0);
        if (!CheckHash(buf, rb, hash)) return 0;
        FileClose(&fp);
    } else {
        return 0;
    }
	
    return 1;
}

void downgradeMSET()
{
	File dg;
	char *dgpath = "0:msetdg.bin";
	unsigned int titleid_low = 0x00040010;
	unsigned int titleid_high[] = { 0x00020000, 0x00021000, 0x00022000/*, 0x00026000, 0x00027000, 0x00028000*/}; //JPN, USA, EUR, CHN, KOR, TWN
	unsigned int mset_hash[] = { 0x57358F14, 0xA28EAD9F, 0x530C345B };   //JPN, USA, EUR
	unsigned int supported_regions = sizeof(titleid_high)/sizeof(titleid_high[0]);
	
	ConsoleInit();
	ConsoleSetTitle("MSET Downgrader");
	print("Opening MSET app...\n"); ConsoleShow();
	
	if (CheckRegion(SYS_NAND) == 0)
	{
		if (region < supported_regions)
		{
			if (FindApp(titleid_low, titleid_high[region], 1)) // SysNAND only
			{
				FileOpen(&dg, cntpath, 0);
				unsigned int check_val;
				FileRead(&dg, &check_val, 4, 0x130);
				FileClose(&dg);
				
				if (check_val != 0)
				{
					if (checkDgFile(dgpath, mset_hash[region]))
					{
						print("Opening downgrade pack...\n"); ConsoleShow();
						
						FileOpen(&dg, dgpath, 0);
						unsigned int dgsize = FileGetSize(&dg);
						unsigned char *buf = 0x21000000;
						FileRead(&dg, buf, dgsize, 0);
						
						/* Downgrade pack decryption */
						u8 iv[0x10] = {0};
						u8 Key[0x10] = {0};
						
						GetTitleKey(&Key, titleid_low, titleid_high[region]);
						
						aes_context aes_ctxt;
						aes_setkey_dec(&aes_ctxt, Key, 0x80);
						aes_crypt_cbc(&aes_ctxt, AES_DECRYPT, dgsize, iv, buf, buf);
						
						FileWrite(&dg, buf, dgsize, 0);
						FileClose(&dg);
						
						if (*((unsigned int*)(buf + 0x100)) == 0x4843434E)
						{
							print("Downgrading... "); ConsoleShow();
							FileCopy(cntpath, dgpath);
							print("done!\nRemoving downgrade pack... "); ConsoleShow();
							f_unlink(dgpath);
							print("done.\n"); ConsoleShow();
						} else {
							print("Error: bad downgrade pack.\n"); ConsoleShow();
						}
					} else {
						print("Error reading downgrade pack.\n"); ConsoleShow();
					}
				} else {
					print("MSET is already in its exploitable version.\nThere's no need to downgrade.\n"); ConsoleShow();
				}
			} else {
				print("Error: couldn't find the MSET app.\n"); ConsoleShow();
			}
		} else {
			print("Sorry, the MSET downgrade process is not\n"); ConsoleShow();
			print("compatible with your region *yet*.\n"); ConsoleShow();
		}
	}
	
	print("\nPress A to exit\n");
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

void manageFBI(bool restore)
{
	int drive;
	unsigned int titleid_low = 0x00040010;
	unsigned int titleid_high[6] = { 0x00020300, 0x00021300, 0x00022300, 0x00026300, 0x00027300, 0x00028300 }; //JPN, USA, EUR, CHN, KOR, TWN
	char *backup_path = "rxTools/h&s_backup";
	
	File tmp;
	char path[256] = {0};
	unsigned char *buf = 0x21000000;
	unsigned int size;
	
	unsigned char TmdCntInfoRecSum[32] = {0};
	unsigned char CntInfoRecSum[32] = {0};
	unsigned char TmdCntChnkRecSum[32] = {0};
	unsigned char CntChnkRecSum[32] = {0};
	unsigned char TmdCntDataSum[32] = {0};
	unsigned char CntDataSum[32] = {0};

	if ((drive = NandSwitch()) == UNK_NAND) return;
	
	ConsoleInit();
	ConsoleSetTitle(restore ? "Restore Health&Safety" : "FBI Installation");
	
	if (CheckRegion(drive) == 0)
	{
		if (FindApp(titleid_low, titleid_high[region], drive))
		{
			/* Open the NAND H&S TMD */
			FileOpen(&tmp, tmdpath, 0);
			FileRead(&tmp, buf, 0xB34, 0);
			FileClose(&tmp);
			
			/* Get the title version from the TMD */
			unsigned short tmd_ver = (unsigned short)((buf[0x1DC] << 8) | buf[0x1DD]);
			print("TMD Version: v%u.\n", tmd_ver);
			
			/* Create the Health & Safety data backup directory */
			if (!restore)
			{
				/* Get the stored content size from the TMD */
				unsigned int cntsize = (unsigned int)((buf[0xB10] << 24) | (buf[0xB11] << 16) | (buf[0xB12] << 8) | buf[0xB13]);
				
				/* Open the NAND H&S content file and read it to the memory buffer */
				FileOpen(&tmp, cntpath, 0);
				FileRead(&tmp, buf + 0x1000, cntsize, 0);
				FileClose(&tmp);
				
				f_mkdir(backup_path);
				
				memset(&tmpstr, 0, 256);
				sprintf(tmpstr, "%s/%s", backup_path, regions[region]);
				f_mkdir(tmpstr);
				
				memset(&tmpstr, 0, 256);
				sprintf(tmpstr, "%s/%s/v%u", backup_path, regions[region], tmd_ver);
				f_mkdir(tmpstr);
				
				/* Backup the H&S TMD */
				sprintf(path, "0:%s/%.12s", tmpstr, tmdpath+34);
				if (FileOpen(&tmp, path, 1))
				{
					if (FileWrite(&tmp, buf, 0xB34, 0) == 0xB34)
					{
						FileClose(&tmp);
						print("NAND H&S TMD backup created.\n"); ConsoleShow();
						
						/* Backup the H&S content file */
						memset(&path, 0, 256);
						sprintf(path, "0:%s/%.12s", tmpstr, cntpath+34);
						if (FileOpen(&tmp, path, 1))
						{
							if (FileWrite(&tmp, buf + 0x1000, cntsize, 0) == cntsize)
							{
								FileClose(&tmp);
								print("NAND H&S content backup created.\nEditing Health & Safety Information... "); ConsoleShow();
								
								/* Open the FBI TMD */
								if (FileOpen(&tmp, "0:fbi_inject.tmd", 0))
								{
									size = FileGetSize(&tmp);
									if (size == 0xB34)
									{
										if (FileRead(&tmp, buf, 0xB34, 0) == 0xB34)
										{
											FileClose(&tmp);
											
											/* Get the FBI TMD version and stored content size */
											unsigned short fbi_tmd_ver = (unsigned short)((buf[0x1DC] << 8) | buf[0x1DD]);
											unsigned int fbi_cntsize = (unsigned int)((buf[0xB10] << 24) | (buf[0xB11] << 16) | (buf[0xB12] << 8) | buf[0xB13]);
											
											if (fbi_tmd_ver == tmd_ver)
											{
												/* Get the SHA-256 hashes */
												memcpy(TmdCntInfoRecSum, (void*)(buf + 0x1E4), 32);
												memcpy(TmdCntChnkRecSum, (void*)(buf + 0x208), 32);
												memcpy(TmdCntDataSum, (void*)(buf + 0xB14), 32);
												
												/* Verify the Content Info Record hash */
												sha2((unsigned char*)(buf + 0x204), 0x900, CntInfoRecSum, 0);
												if (memcmp(CntInfoRecSum, TmdCntInfoRecSum, 32) == 0)
												{
													/* Verify the Content Chunk Record hash */
													sha2((unsigned char*)(buf + 0xB04), 0x30, CntChnkRecSum, 0);
													if (memcmp(CntChnkRecSum, TmdCntChnkRecSum, 32) == 0)
													{
														/* Open the FBI content file */
														if (FileOpen(&tmp, "0:fbi_inject.app", 0))
														{
															size = FileGetSize(&tmp);
															if (size == fbi_cntsize)
															{
																if (FileRead(&tmp, buf + 0x1000, fbi_cntsize, 0) == fbi_cntsize)
																{
																	FileClose(&tmp);
																	
																	/* Verify the Content Data hash */
																	sha2((unsigned char*)(buf + 0x1000), fbi_cntsize, CntDataSum, 0);
																	if (memcmp(CntDataSum, TmdCntDataSum, 32) == 0)
																	{
																		/* Now we are ready to rock 'n roll */
																		if (FileCopy(tmdpath, "0:fbi_inject.tmd") == 1)
																		{
																			if (FileCopy(cntpath, "0:fbi_inject.app") == 1)
																			{
																				print("OK!\nDeleting 'fbi_inject.tmd'... "); ConsoleShow();
																				f_unlink("0:fbi_inject.tmd");
																				print("done.\nDeleting 'fbi_inject.app'... "); ConsoleShow();
																				f_unlink("0:fbi_inject.app");
																				print("done.\n"); ConsoleShow();
																			} else {
																				print("\nError injecting FBI content file.\n"); ConsoleShow();
																			}
																		} else {
																			print("\nError injecting FBI TMD.\n"); ConsoleShow();
																		}
																	} else {
																		print("\nError: invalid Content Data hash.\nGot:\n"); ConsoleShow();
																		print_sha256(CntDataSum);
																		print("\nExpected:\n"); ConsoleShow();
																		print_sha256(TmdCntDataSum);
																	}
																} else {
																	FileClose(&tmp);
																	print("\nError reading FBI content file.\n"); ConsoleShow();
																}
															} else {
																FileClose(&tmp);
																print("\nError: invalid FBI content file size.\nGot: %u / Expected: %u\n", size, fbi_cntsize); ConsoleShow();
															}
														} else {
															print("\nError opening FBI content file.\n"); ConsoleShow();
														}
													} else {
														print("\nError: invalid Content Chunk Record hash.\nGot:\n"); ConsoleShow();
														print_sha256(CntChnkRecSum);
														print("\nExpected:\n"); ConsoleShow();
														print_sha256(TmdCntChnkRecSum);
													}
												} else {
													print("\nError: invalid Content Info Record hash.\nGot:\n"); ConsoleShow();
													print_sha256(CntInfoRecSum);
													print("\nExpected:\n"); ConsoleShow();
													print_sha256(TmdCntInfoRecSum);
												}
											} else {
												print("\nError: invalid FBI TMD version.\nGot: v%u / Expected: v%u\n", fbi_tmd_ver, tmd_ver); ConsoleShow();
											}
										} else {
											FileClose(&tmp);
											print("\nError reading FBI TMD.\n"); ConsoleShow();
										}
									} else {
										FileClose(&tmp);
										print("\nError: invalid FBI TMD size.\nGot: %u / Expected: %u\n", size, 0xB34); ConsoleShow();
									}
								} else {
									print("\nError opening FBI TMD.\n"); ConsoleShow();
								}
							} else {
								FileClose(&tmp);
								print("Error writing H&S content backup.\n"); ConsoleShow();
							}
						} else {
							print("Error creating H&S content backup.\n"); ConsoleShow();
						}
					} else {
						FileClose(&tmp);
						print("Error writing H&S TMD backup.\n"); ConsoleShow();
					}
				} else {
					print("Error creating H&S TMD backup.\n"); ConsoleShow();
				}
			} else {
				print("Restoring Health & Safety Information... "); ConsoleShow();
				
				memset(&tmpstr, 0, 256);
				sprintf(tmpstr, "%s/%s/v%u", backup_path, regions[region], tmd_ver);
				
				/* Open the H&S TMD backup */
				sprintf(path, "0:%s/%.12s", tmpstr, tmdpath+34);
				if (FileOpen(&tmp, path, 0))
				{
					size = FileGetSize(&tmp);
					if (size == 0xB34)
					{
						if (FileRead(&tmp, buf, 0xB34, 0) == 0xB34)
						{
							FileClose(&tmp);
							
							/* Get the H&S TMD version and stored content size */
							unsigned short hs_tmd_ver = (unsigned short)((buf[0x1DC] << 8) | buf[0x1DD]);
							unsigned int hs_cntsize = (unsigned int)((buf[0xB10] << 24) | (buf[0xB11] << 16) | (buf[0xB12] << 8) | buf[0xB13]);
							
							if (hs_tmd_ver == tmd_ver)
							{
								/* Get the SHA-256 hashes */
								memcpy(TmdCntInfoRecSum, (void*)(buf + 0x1E4), 32);
								memcpy(TmdCntChnkRecSum, (void*)(buf + 0x208), 32);
								memcpy(TmdCntDataSum, (void*)(buf + 0xB14), 32);
								
								/* Verify the Content Info Record hash */
								sha2((unsigned char*)(buf + 0x204), 0x900, CntInfoRecSum, 0);
								if (memcmp(CntInfoRecSum, TmdCntInfoRecSum, 32) == 0)
								{
									/* Verify the Content Chunk Record hash */
									sha2((unsigned char*)(buf + 0xB04), 0x30, CntChnkRecSum, 0);
									if (memcmp(CntChnkRecSum, TmdCntChnkRecSum, 32) == 0)
									{
										char path2[256] = {0};
										sprintf(path2, "0:%s/%.12s", tmpstr, cntpath+34);
										
										/* Open the H&S content file */
										if (FileOpen(&tmp, path2, 0))
										{
											size = FileGetSize(&tmp);
											if (size == hs_cntsize)
											{
												if (FileRead(&tmp, buf + 0x1000, hs_cntsize, 0) == hs_cntsize)
												{
													FileClose(&tmp);
													
													/* Verify the Content Data hash */
													sha2((unsigned char*)(buf + 0x1000), hs_cntsize, CntDataSum, 0);
													if (memcmp(CntDataSum, TmdCntDataSum, 32) == 0)
													{
														/* Let's do this */
														if (FileCopy(tmdpath, path) == 1)
														{
															if (FileCopy(cntpath, path2) == 1)
															{
																print("OK!\nDeleting backup data... "); ConsoleShow();
																f_unlink(path);
																f_unlink(path2);
																print("done.\n"); ConsoleShow();
															} else {
																print("\nError restoring H&S content file.\n"); ConsoleShow();
															}
														} else {
															print("\nError restoring H&S TMD.\n"); ConsoleShow();
														}
													} else {
														print("\nError: invalid Content Data hash.\nGot:\n"); ConsoleShow();
														print_sha256(CntDataSum);
														print("\nExpected:\n"); ConsoleShow();
														print_sha256(TmdCntDataSum);
													}
												} else {
													FileClose(&tmp);
													print("\nError reading H&S content file backup.\n");
												}
											} else {
												FileClose(&tmp);
												print("\nError: invalid H&S content file backup size.\nGot: v%u / Expected: v%u\n", size, hs_cntsize); ConsoleShow();
											}
										} else {
											print("\nError opening H&S content file backup.\n");
										}
									} else {
										print("\nError: invalid Content Chunk Record hash.\nGot:\n"); ConsoleShow();
										print_sha256(CntChnkRecSum);
										print("\nExpected:\n"); ConsoleShow();
										print_sha256(TmdCntChnkRecSum);
									}
								} else {
									print("\nError: invalid Content Info Record hash.\nGot:\n"); ConsoleShow();
									print_sha256(CntInfoRecSum);
									print("\nExpected:\n"); ConsoleShow();
									print_sha256(TmdCntInfoRecSum);
								}
							} else {
								print("\nError: invalid H&S TMD backup version.\nGot: v%u / Expected: v%u\n", hs_tmd_ver, tmd_ver); ConsoleShow();
							}
						} else {
							FileClose(&tmp);
							print("\nError reading H&S TMD backup.\n");
						}
					} else {
						FileClose(&tmp);
						print("\nError: invalid H&S TMD backup size.\nGot: %u / Expected: %u\n", size, 0xB34); ConsoleShow();
					}
				} else {
					print("\nError opening H&S TMD backup.\n"); ConsoleShow();
				}
			}
		} else {
			print("Error: couldn't find H&S TMD/content.\n"); ConsoleShow();
		}
	}
	
	print("\nPress A to exit.");
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
