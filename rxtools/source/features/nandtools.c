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
    TCHAR* name;
    TCHAR* path;
} CoolFiles[] = {
    {_T("movable.sed"), _T("private")},
    {_T("SecureInfo_A"), _T("rw/sys")},
    {_T("LocalFriendCodeSeed_B"), _T("rw/sys")},
    {_T("rand_seed"), _T("rw/sys")},
    {_T("ticket.db"), _T("dbs")},
    {_T("import.db"), _T("dbs")},
};

static Menu CoolFilesMenu = {
	L"Choose the file to work on",
	.Option = (MenuEntry[nCoolFiles]){
		{ L" movable.sed", &SelectFile, L"fil0.bin" },
		{ L" SecureInfo_A", &SelectFile, L"fil1.bin" },
		{ L" LocalFriendCodeSeed_B", &SelectFile, L"fil2.bin" },
		{ L" rand_seed", &SelectFile, L"fil3.bin" },
		{ L" ticket.db", &SelectFile, L"fil4.bin" },
		{ L" import.db", &SelectFile, L"fil5.bin" },
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

	wchar_t dest[_MAX_LFN], tmpstr[_MAX_LFN];
	swprintf(dest, _MAX_LFN, L"rxTools/%ls", CoolFiles[selectedFile].name);
	swprintf(tmpstr, _MAX_LFN, L"%d:%ls/%ls",
		nandtype, CoolFiles[selectedFile].path,
		CoolFiles[selectedFile].name);
	print(strings[STR_DUMPING], tmpstr, dest);
	ConsoleShow();

	unsigned int res = FSFileCopy(dest, tmpstr);
	if (res != 0 && (selectedFile == 1 || selectedFile == 2)){
		if (selectedFile == 1)
		{
			/* Fix for SecureInfo_B */
			swprintf(dest, _MAX_LFN, L"rxTools/%.11ls%c",
				CoolFiles[selectedFile].name, 'B');

			swprintf(tmpstr, _MAX_LFN, L"%d:%ls/%.11ls%c",
				nandtype, CoolFiles[selectedFile].path,
				CoolFiles[selectedFile].name, 'B');
		}
		else if (selectedFile == 2)
		{
			/* Fix for LocalFriendCodeSeed_A */
			swprintf(dest, _MAX_LFN, L"rxTools/%.20ls%c",
				CoolFiles[selectedFile].name, 'A');
			swprintf(tmpstr, _MAX_LFN, L"%d:%ls/%.20ls%c",
				nandtype, CoolFiles[selectedFile].path,
				CoolFiles[selectedFile].name, 'A');
		}
		print(strings[STR_FAILED]);
		print(strings[STR_DUMPING], tmpstr, dest);
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

	wchar_t dest[_MAX_LFN], tmpstr[_MAX_LFN];

	swprintf(tmpstr, _MAX_LFN, L"rxTools/%ls",
		CoolFiles[selectedFile].name);

	swprintf(dest, _MAX_LFN, L"%d:%ls/%ls",
		nandtype, CoolFiles[selectedFile].path,
		CoolFiles[selectedFile].name);

	print(strings[STR_INJECTING], tmpstr, dest);
	ConsoleShow();

	unsigned int res = FSFileCopy(dest, tmpstr);
	if (res != 0 && (selectedFile == 1 || selectedFile == 2)){
		if (selectedFile == 1)
		{
			/* Fix for SecureInfo_B */
			swprintf(tmpstr, _MAX_LFN, L"rxTools/%.11ls%lc",
				CoolFiles[selectedFile].name, L'B');

			swprintf(dest, _MAX_LFN, L"%d:%ls/%.11ls%lc",
				nandtype, CoolFiles[selectedFile].path,
				CoolFiles[selectedFile].name, L'B');
		}
		else if (selectedFile == 2)
		{
			swprintf(tmpstr, _MAX_LFN, L"rxTools/%.20s%lc",
				CoolFiles[selectedFile].name, L'A');

			swprintf(dest, _MAX_LFN, L"%d:%ls/%.20ls%lc",
				nandtype, CoolFiles[selectedFile].path,
				CoolFiles[selectedFile].name, L'A');
		}
		print(strings[STR_FAILED]);
		print(strings[STR_INJECTING], tmpstr, dest);
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
