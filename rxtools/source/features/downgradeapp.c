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
#include "stdio.h"
#include "filepack.h"

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

char* getContentAppPath(){
	return (char*)&cntpath;
}

char* getTMDAppPath(){
	return (char*)&tmdpath;
}

void print_sha256(unsigned char hash[32])
{
	int i;
	for (i = 0; i < 32; i++)
	{
		print("%02X", hash[i]);
		
		if (i == 16)
		{
			/* Continue printing the SHA-256 sum in a new line after 34 characters (17 bytes) */
			print("\n"); ConsoleShow();
		}
	}
	
	ConsoleShow();
}

int FindApp(unsigned int tid_low, unsigned int tid_high, int drive)
{
    char *folder = (char*)&tmpstr;
    memset(folder, 0, 256);
	
    DIR* curDir = &myDir;
	memset((unsigned char*)curDir, 0, sizeof(DIR));
	
	FILINFO *myInfo = &curInfo;
    memset((unsigned char*)myInfo, 0, sizeof(FILINFO));
	myInfo->fname[0] = 'A';
	
    sprintf(folder, "%d:title/%08x/%08x/content", drive, tid_low, tid_high);
	
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
					sprintf(tmdpath, "%s/%s", folder, myInfo->fname);
					sprintf(cntpath, "%s/%08x.app", folder, bswap_32(tmd_entry.id));
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
			print("Error.\nProcess failed!\n"); ConsoleShow();
			return -1;
		}
	}
	
	FileRead(&secureinfo, &region, 1, 0x100);
	FileClose(&secureinfo);
	
	if (region > 0x06)
	{
		print("Error: unsupported region.\nProcess failed!\n"); ConsoleShow();
		return -1;
	} else {
		/* Avoid problems with the unused "AUS" region code */
		if (region >= 3) region--;
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
		FileClose(&fp);
		if (!CheckHash(buf, rb, hash)) return 0;
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
	unsigned int titleid_high[6] = { 0x00020000, 0x00021000, 0x00022000, 0x00026000, 0x00027000, 0x00028000 }; //JPN, USA, EUR, CHN, KOR, TWN
	unsigned int mset_hash[10] = { 0x96AEC379, 0xED315608, 0x3387F2CD, 0xEDAC05D7, 0xACC1BE62, 0xF0FF9F08, 0x565BCF20, 0xA04654C6, 0xAFD07166, 0xD40B12F4 }; //JPN, USA, EUR, CHN, KOR, TWN
	unsigned short mset_ver[10] = { 3074, 5127, 3078, 5128, 3075, 5127, 8, 1026, 2049, 8 };
	unsigned short mset_dg_ver = 0;
	unsigned int buttonInput;
	unsigned int checkLoop = 0;

	ConsoleInit();
	ConsoleSetTitle("         MSET DOWNGRADER");

	CheckRegionSilent(SYS_NAND);

	print("What would you like to\nDowngrade to?\n\n"); ConsoleShow();
	print("[X] 4.x MSET\n[Y] 5.x/6.x MSET\n[B] Cancel\n\n"); ConsoleShow();

	while( checkLoop < 1 )
	{
		buttonInput = GetInput();
		//print("buttonInput is:  %u\n", buttonInput); ConsoleShow(); //debug

		if (buttonInput == 4294964224)
		{
			if (region == 0)
			{
				mset_dg_ver = 0;
			}
			else if (region == 1)
			{
				mset_dg_ver = 2;
			}
			else if (region == 2)
			{
				mset_dg_ver = 4;
			}
			else if (region == 3)
			{
				mset_dg_ver = 6;
			}
			else if (region == 4)
			{
				mset_dg_ver = 7;
			}
			else if (region == 5)
			{
				mset_dg_ver = 9;
			}
			else
			{
				print("region is:  %u\n", region); ConsoleShow();
				print("Unsupported Region!\n"); ConsoleShow();
				mset_dg_ver = 0;
			}
			checkLoop = 1;
		}
		else if (buttonInput == 4294965248)
		{
			if (region == 0)
			{
				mset_dg_ver = 1;
			}
			else if (region == 1)
			{
				mset_dg_ver = 3;
			}
			else if (region == 2)
			{
				mset_dg_ver = 5;
			}
			else if (region == 3)
			{
				mset_dg_ver = 6;
				print("CHN Region Detected!\nFalling back to 4.x MSET\n"); ConsoleShow();
			}
			else if (region == 4)
			{
				mset_dg_ver = 8;
			}
			else if (region == 5)
			{
				mset_dg_ver = 9;
				print("TWN Region Detected!\nFalling back to 4.x MSET\n"); ConsoleShow();
			}
			else
			{
				print("Unsupported Region!\n"); ConsoleShow();
				mset_dg_ver = 0;
			}
			checkLoop = 1;
		}
		else if (buttonInput == 4294963202)
		{
			checkLoop = 1;
			print("Operation Canceled!\n"); ConsoleShow();
		}
	}


	if(buttonInput != 4294963202)
	{
	
		print("Opening MSET app...\n"); ConsoleShow();
		
		if (CheckRegion(SYS_NAND) == 0)
		{
			if (FindApp(titleid_low, titleid_high[region], SYS_NAND)) // SysNAND only
			{
				if (FileOpen(&dg, tmdpath, 0))
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
						if (FileOpen(&dg, cntpath, 0))
						{
							unsigned int check_val;
							FileRead(&dg, &check_val, 4, 0x130);
							FileClose(&dg);
							
							if (check_val != 0)
							{
								if (checkDgFile(dgpath, mset_hash[mset_dg_ver]))
								{
									print("Opening downgrade pack... "); ConsoleShow();
									if (FileOpen(&dg, dgpath, 0))
									{
										print("OK!\n"); ConsoleShow();
										
										unsigned int dgsize = FileGetSize(&dg);
										unsigned char *buf = (unsigned char*)0x21000000;
										FileRead(&dg, buf, dgsize, 0);
										
										/* Downgrade pack decryption */
										u8 iv[0x10] = {0};
										u8 Key[0x10] = {0};
										
										GetTitleKey(&Key[0], titleid_low, titleid_high[region], SYS_NAND);
										
										aes_context aes_ctxt;
										aes_setkey_dec(&aes_ctxt, Key, 0x80);
										aes_crypt_cbc(&aes_ctxt, AES_DECRYPT, dgsize, iv, buf, buf);
										
										FileWrite(&dg, buf, dgsize, 0);
										FileClose(&dg);
										
										if (*((unsigned int*)(buf + 0x100)) == 0x4843434E) // "NCCH" magic word
										{
											print("Downgrading... "); ConsoleShow();
											if (FSFileCopy(cntpath, dgpath) == 0)
											{
												print("done!\nRemoving downgrade pack... "); ConsoleShow();
												f_unlink(dgpath);
												print("done.\n"); ConsoleShow();
											} else {
												print("\nError downgrading MSET content.\nRemoving downgrade pack... "); ConsoleShow();
												f_unlink(dgpath);
												print("done.\n"); ConsoleShow();
											}
										} else {
											print("Error: bad downgrade pack.\n"); ConsoleShow();
										}
									} else {
										print("Error.\n"); ConsoleShow();
									}
								} else {
									print("Error: bad downgrade pack.\n"); ConsoleShow();
								}
							} else {
								print("Your MSET version is exploitable.\nDowngrade isn't necessary.\n"); ConsoleShow();
							}
						} else {
							print("Error opening MSET content file.\n"); ConsoleShow();
						}
					} else {
						print("Your MSET version is exploitable.\nDowngrade isn't necessary.\n"); ConsoleShow();
					}
				} else {
					print("Error opening MSET TMD.\n"); ConsoleShow();
				}
			} else {
				print("Error: couldn't find MSET data.\n"); ConsoleShow();
			}
		}
	}
	
	print("\nPress A to exit\n\n\n");
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
	char path2[256] = {0};
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

	unsigned int buttonInput;
	unsigned short checkLoop;
	
	if ((drive = NandSwitch()) == UNK_NAND) return;
	
	ConsoleInit();
	ConsoleSetTitle(restore ? "     RESTORE HEALTH & SAFETY" : "         FBI INSTALLATION");
	
	if (CheckRegion(drive) == 0)
	{
		if (FindApp(titleid_low, titleid_high[region], drive))
		{
			/* Open the NAND H&S TMD */
			FileOpen(&tmp, tmdpath, 0);
			FileRead(&tmp, buf, 0xB34, 0);
			FileClose(&tmp);
			
			/* Get the title version from the TMD */
			tmd_ver = (unsigned short)((buf[0x1DC] << 8) | buf[0x1DD]);
			print("TMD Version: v%u.\n", tmd_ver);
			
			if (!restore)
			{
				/* Get the stored content size from the TMD */
				unsigned int cntsize = (unsigned int)((buf[0xB10] << 24) | (buf[0xB11] << 16) | (buf[0xB12] << 8) | buf[0xB13]);
				
				/* Open the NAND H&S content file and read it to the memory buffer */
				FileOpen(&tmp, cntpath, 0);
				FileRead(&tmp, buf + 0x1000, cntsize, 0);
				FileClose(&tmp);
				
				/* Create the Health & Safety data backup directory */
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
					size = FileWrite(&tmp, buf, 0xB34, 0);
					FileClose(&tmp);
					if (size == 0xB34)
					{
						print("NAND H&S TMD backup created.\n"); ConsoleShow();
						
						/* Backup the H&S content file */
						memset(&path, 0, 256);
						sprintf(path, "0:%s/%.12s", tmpstr, cntpath+34);
						if (FileOpen(&tmp, path, 1))
						{
							size = FileWrite(&tmp, buf + 0x1000, cntsize, 0);
							FileClose(&tmp);
							if (size == cntsize)
							{
								print("NAND H&S content backup created.\n"); ConsoleShow();
							} else {
								print("Error writing H&S content backup.\n"); ConsoleShow();
								goto out;
							}
						} else {
							print("Error creating H&S content backup.\n"); ConsoleShow();
							goto out;
						}
					} else {
						print("Error writing H&S TMD backup.\n"); ConsoleShow();
						goto out;
					}
				} else {
					print("Error creating H&S TMD backup.\n"); ConsoleShow();
					goto out;
				}
				
				/* Generate the FBI data paths */
				sprintf(path, "0:fbi_inject.tmd");
				sprintf(path2, "0:fbi_inject.app");
				
				print("Editing H&S Information... "); ConsoleShow();
			} else {
				/* Generate the H&S backup data paths */
				memset(&tmpstr, 0, 256);
				sprintf(tmpstr, "%s/%s/v%u", backup_path, regions[region], tmd_ver);
				sprintf(path, "0:%s/%.12s", tmpstr, tmdpath+34);
				sprintf(path2, "0:%s/%.12s", tmpstr, cntpath+34);
				
				print("Restoring H&S Information... "); ConsoleShow();
			}
			
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
											if (FSFileCopy(tmdpath, path) == 0)
											{
												if (FSFileCopy(cntpath, path2) == 0)
												{
													print("\n\nWhat would you like to do?\n"); ConsoleShow();
													print("[B] Keep %s Data\n", restore ? "backup": "FBI injection"); ConsoleShow();
													print("[X] Delete %s Data\n\n", restore ? "backup": "FBI injection"); ConsoleShow();
													checkLoop = 0;

													while (checkLoop < 1)
													{
														buttonInput = GetInput();

														if (buttonInput == 4294963202)
														{
															print("OK!\n\nKeeping %s data.\n", restore ? "backup" : "FBI injection"); ConsoleShow();
															checkLoop = 1;
														}
														else if (buttonInput == 4294964224)
														{
															print("OK!\n\nDeleting %s data... ", restore ? "backup" : "FBI injection"); ConsoleShow();
															f_unlink(path);
															f_unlink(path2);
															print("OK!\n"); ConsoleShow();
															checkLoop = 1;
														}
													}
												} else {
													print("\nError %s content file.\n", restore ? "restoring H&S" : "injecting FBI"); ConsoleShow();
												}
											} else {
												print("\nError %s TMD.\n", restore ? "restoring H&S" : "injecting FBI"); ConsoleShow();
											}
										} else {
											print("\nError: invalid Content Data hash.\nGot:\n"); ConsoleShow();
											print_sha256(CntDataSum);
											print("\nExpected:\n"); ConsoleShow();
											print_sha256(TmdCntDataSum);
										}
									} else {
										FileClose(&tmp);
										print("\nInvalid %s content size.\nGot: v%u / Expected: v%u\n", restore ? "backup" : "FBI", size, sd_cntsize); ConsoleShow();
									}
								} else {
									print("\nError opening %s content.\n", restore ? "backup" : "FBI");
								}
							} else {
								print("\nError: invalid Content Chunk hash.\nGot:\n"); ConsoleShow();
								print_sha256(CntChnkRecSum);
								print("\nExpected:\n"); ConsoleShow();
								print_sha256(TmdCntChnkRecSum);
							}
						} else {
							print("\nError: invalid Content Info hash.\nGot:\n"); ConsoleShow();
							print_sha256(CntInfoRecSum);
							print("\nExpected:\n"); ConsoleShow();
							print_sha256(TmdCntInfoRecSum);
						}
					} else {
						print("\nError: invalid %s TMD version.\nGot: v%u / Expected: v%u\n", restore ? "backup" : "FBI", sd_tmd_ver, tmd_ver); ConsoleShow();
					}
				} else {
					FileClose(&tmp);
					print("\nError: invalid %s TMD size.\nGot: %u / Expected: %u\n", restore ? "backup" : "FBI", size, 0xB34); ConsoleShow();
				}
			} else {
				print("\nError opening %s TMD.\n", restore ? "backup" : "FBI"); ConsoleShow();
			}
		} else {
			print("Error: couldn't find H&S data.\n"); ConsoleShow();
		}
	}
	
out:
	print("\nPress A to exit.\n\n\n");
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
