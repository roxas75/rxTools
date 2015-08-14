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

#include <stdlib.h>
#include "menu.h"
#include "nandtools.h"
#include "console.h"
#include "draw.h"
#include "lang.h"
#include "hid.h"
#include "fs.h"
#include "screenshot.h"
#include "padgen.h"
#include "crypto.h"
#include "ncch.h"
#include "NandDumper.h"
#include "stdio.h"

#define nCoolFiles sizeof(CoolFiles)/sizeof(CoolFiles[0])

uint32_t selectedFile;
void SelectFile();

static struct {
    char* name;
    char* path;
} CoolFiles[] = {
    {"movable.sed", "private"},
    {"SecureInfo_A", "rw/sys"},
    {"LocalFriendCodeSeed_B", "rw/sys"},
    {"rand_seed", "rw/sys"},
    {"ticket.db", "dbs"},
};

static Menu CoolFilesMenu = {
	L"Choose the file to work on",
	.Option = (MenuEntry[nCoolFiles]){
		{ L" movable.sed", &SelectFile, "fil0.bin" },
		{ L" SecureInfo_A", &SelectFile, "fil1.bin" },
		{ L" LocalFriendCodeSeed_B", &SelectFile, "fil2.bin" },
		{ L" rand_seed", &SelectFile, "fil3.bin" },
		{ L" ticket.db", &SelectFile, "fil4.bin" },
	},
	nCoolFiles,
	0,
	0
};

void SelectFile(){
    selectedFile = CoolFilesMenu.Current;
}

void dumpCoolFiles()
{
	int nandtype = NandSwitch();
	if (nandtype == UNK_NAND) return;

	selectedFile = -1;
	MenuInit(&CoolFilesMenu);
	MenuShow();

	while (true)
	{
		uint32_t pad_state = InputWait();
		if (pad_state & BUTTON_DOWN) MenuNextSelection();
		if (pad_state & BUTTON_UP) MenuPrevSelection();
		if (pad_state & BUTTON_A) { MenuSelect(); break; }
		if (pad_state & BUTTON_B) break;
		TryScreenShot();
		MenuShow();
	}

	if (selectedFile == -1) return;
	ConsoleInit();
	ConsoleSetTitle(strings[STR_DUMP], strings[STR_FILES]);

	char dest[256], tmpstr[sizeof(dest)];
	wchar_t wsrc[sizeof(tmpstr)];
	sprintf(dest, "rxTools/%s", CoolFiles[selectedFile].name);
	sprintf(tmpstr, "%d:%s/%s", nandtype, CoolFiles[selectedFile].path, CoolFiles[selectedFile].name);
	mbstowcs(wsrc, tmpstr, sizeof(tmpstr));
	print(strings[STR_DUMPING], wsrc, dest);
	ConsoleShow();

	unsigned int res = FSFileCopy(dest, tmpstr);
	if (res != 0 && (selectedFile == 1 || selectedFile == 2)){
		if (selectedFile == 1)
		{
			/* Fix for SecureInfo_B */
			sprintf(dest, "rxTools/%.11s%c", CoolFiles[selectedFile].name, 'B');
			sprintf(tmpstr, "%d:%s/%.11s%c", nandtype, CoolFiles[selectedFile].path, CoolFiles[selectedFile].name, 'B');
		}
		else if (selectedFile == 2)
		{
			/* Fix for LocalFriendCodeSeed_A */
			sprintf(dest, "rxTools/%.20s%c", CoolFiles[selectedFile].name, 'A');
			sprintf(tmpstr, "%d:%s/%.20s%c", nandtype, CoolFiles[selectedFile].path, CoolFiles[selectedFile].name, 'A');
		}
		mbstowcs(wsrc, tmpstr, sizeof(tmpstr));
		print(strings[STR_FAILED]);
		print(strings[STR_DUMPING], wsrc, dest);
		ConsoleShow();
		res = FSFileCopy(dest, tmpstr);
	}

	switch ((res >> 8) & 0xFF)
	{
		case 0:
			print(strings[STR_COMPLETED]);
			break;
		case 1:
			print(strings[STR_ERROR_OPENING], tmpstr);
			break;
		case 2:
			print(strings[STR_ERROR_CREATING], dest);
			break;
		case 3:
		case 4:
			print(strings[STR_ERROR_READING], tmpstr);
			break;
		case 5:
		case 6:
			print(strings[STR_ERROR_WRITING], dest);
			break;
		default:
			print(strings[STR_FAILED]);
			break;
	}

	print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

void restoreCoolFiles()
{
	int nandtype = NandSwitch();
	if (nandtype == UNK_NAND) return;

	selectedFile = -1;
	MenuInit(&CoolFilesMenu);
	MenuShow();
	while (true)
	{
		uint32_t pad_state = InputWait();
		if (pad_state & BUTTON_DOWN) MenuNextSelection();
		if (pad_state & BUTTON_UP) MenuPrevSelection();
		if (pad_state & BUTTON_A) { MenuSelect(); break; }
		if (pad_state & BUTTON_B) break;
		TryScreenShot();
		MenuShow();
	}

	if (selectedFile == -1) return;
	ConsoleInit();
	ConsoleSetTitle(strings[STR_INJECT], strings[STR_FILES]);

	char dest[256], tmpstr[sizeof(dest)];
	wchar_t wdest[sizeof(dest)];
	sprintf(tmpstr, "rxTools/%s", CoolFiles[selectedFile].name);
	sprintf(dest, "%d:%s/%s", nandtype, CoolFiles[selectedFile].path, CoolFiles[selectedFile].name);
	mbstowcs(wdest, dest, sizeof(dest));
	print(strings[STR_INJECTING], tmpstr, wdest);
	ConsoleShow();

	unsigned int res = FSFileCopy(dest, tmpstr);
	if (res != 0 && (selectedFile == 1 || selectedFile == 2)){
		if (selectedFile == 1)
		{
			/* Fix for SecureInfo_B */
			sprintf(tmpstr, "rxTools/%.11s%c", CoolFiles[selectedFile].name, 'B');
			sprintf(dest, "%d:%s/%.11s%c", nandtype, CoolFiles[selectedFile].path, CoolFiles[selectedFile].name, 'B');
		}
		else if (selectedFile == 2)
		{
			sprintf(tmpstr, "rxTools/%.20s%c", CoolFiles[selectedFile].name, 'A');
			sprintf(dest, "%d:%s/%.20s%c", nandtype, CoolFiles[selectedFile].path, CoolFiles[selectedFile].name, 'A');
		}
		mbstowcs(wdest, dest, sizeof(dest));
		print(strings[STR_FAILED]);
		print(strings[STR_INJECTING], tmpstr, wdest);
		ConsoleShow();
		res = FSFileCopy(dest, tmpstr);
	}

	switch ((res >> 8) & 0xFF)
	{
		case 0:
			print(strings[STR_COMPLETED]);
			break;
		case 1:
			print(strings[STR_ERROR_OPENING], tmpstr);
			break;
		case 2:
			print(strings[STR_ERROR_CREATING], dest);
			break;
		case 3:
		case 4:
			print(strings[STR_ERROR_READING], tmpstr);
			break;
		case 5:
		case 6:
			print(strings[STR_ERROR_WRITING], dest);
			break;
		default:
			print(strings[STR_FAILED]);
			break;
	}

	print(strings[STR_PRESS_BUTTON_ACTION], strings[STR_BUTTON_A], strings[STR_CONTINUE]);
	ConsoleShow();
	WaitForButton(BUTTON_A);
}
