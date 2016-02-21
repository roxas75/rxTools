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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <lib/cfg.h>
#include <lib/lang.h>
#include <features/AdvancedFileManager.h>
#include <lib/draw.h>
#include <lib/hid.h>
#include <lib/fs.h>
#include <lib/menu.h>
#include <features/firm.h>
#include <lib/console.h>

 #define FILE_ACTIONS 2

//---- GLOBAL VARIABLES ----
int i;
void *screentmp = (void*)0x27000000;


size_t AdvFileManagerList(const TCHAR *path, TCHAR ***ls) {
	size_t count = 0;

	if (wcscmp(path, L""))
	{
		DIR myDir;
		FILINFO curInfo;
		memset(&myDir, 0, sizeof(DIR));
		memset(&curInfo, 0, sizeof(FILINFO));
		FILINFO *myInfo = &curInfo;

		int res = f_opendir(&myDir, path);
		if (res == FR_NO_FILE) {
			print(strings[STR_ERROR_OPENING], path);
			return 0;
		}
		*ls = NULL;
		*ls = calloc(1000, sizeof(char *));
		count = 0;
		f_readdir(&myDir, myInfo);

		while (!myInfo->fname[0] == 0)
		{
			(*ls)[count++] = wcsdup(myInfo->fname);
			f_readdir(&myDir, myInfo);
		}

		f_closedir(&myDir);
	}
	else
	{
		*ls = calloc(3, sizeof(char *));
		(*ls)[count++] = wcsdup(L"0");
		(*ls)[count++] = wcsdup(L"1");
		(*ls)[count++] = wcsdup(L"2");
	}
	return count;
}

void AdvFileManagerShow(panel_t* Panel, int x){

	//Title
	wchar_t tmp[_MAX_LFN];
	swprintf(tmp, _MAX_LFN, L"%ls", Panel->dir);
	DrawString(screentmp, tmp, 15 + x, FONT_HEIGHT * 2, ConsoleGetTextColor(), TRANSPARENT);

	//FileList
	if (Panel->count != 0)
	{
		int n = 0;
		int divisions = Panel->count % 10 == 0 ? Panel->count / 10 : Panel->count / 10 + 1;
		int list[divisions];
		for (n = 0; n<divisions; n++)
		{
			if (n == divisions - 1 && Panel->count % 10 != 0)list[n] = Panel->count - (Panel->count / 10) * 10;
			else list[n] = 10;
		}

		int i = 0;
		for (i = Panel->beginning; i < Panel->beginning + list[(Panel->pointer / 10)]; i++) {
			swprintf(tmp, _MAX_LFN, L"%ls %ls", (i == Panel->pointer && Panel->enabled == 1) ? strings[STR_CURSOR] : strings[STR_NO_CURSOR], Panel->files[i]);
			DrawString(screentmp, tmp, 15 + x, FONT_HEIGHT * 3 + FONT_HEIGHT * (i - Panel->beginning + 1), ConsoleGetTextColor(), TRANSPARENT);
		}
	}
}

void AdvFileManagerNextSelection(panel_t* Panel){
	if (Panel->pointer != Panel->count - 1) Panel->pointer++;
}

void AdvFileManagerPrevSelection(panel_t* Panel){
	if (Panel->pointer != 0) Panel->pointer--;
}

void AdvFileManagerBack(panel_t* Panel){
	//go back
	int u;
	if (Panel->openedFolder == 1) wcscpy(Panel->dir, L"");
	else{
		Panel->dir[wcslen(Panel->dir) - 1] = 'f'; //just a casual letter
		for (u = wcslen(Panel->dir); u >= 0; u--)
				if (Panel->dir[u] == '/' || Panel->dir[u] == ':') { Panel->dir[u] = '\0'; break; }
	}
	Panel->beginning = 0;
	Panel->pointer = 0;
	Panel->openedFolder--;
	Panel->count = AdvFileManagerList(Panel->dir, &Panel->files);
}


void AdvFileManagerSelect(panel_t* Panel){
	//enter file/folder
	int u;
	int isafile = 0;
	for (u = 0; u < wcslen(Panel->files[Panel->pointer]); u++)
	{
		if (Panel->files[Panel->pointer][u] == '.') isafile = 1;
	}
	if (isafile){
		//Open a file - FIRMS only for now
		wchar_t filePath[_MAX_LFN];
		swprintf(filePath, _MAX_LFN, L"%ls%ls%ls", 
			Panel->dir,                                 // Opened folder
			Panel->openedFolder == 0 ? L"" : L"/", 		// '/' or nothing
			Panel->files[Panel->pointer]);              // File name

		AdvFileManagerFileAction(filePath);
	}
	else
	{
		//enter folder
		if (Panel->openedFolder != 0) wcscat(Panel->dir, L"/");
		wcscat(Panel->dir, Panel->files[Panel->pointer]);
		if (Panel->openedFolder == 0) wcscat(Panel->dir, L":");
		Panel->beginning = 0;
		Panel->pointer = 0;
		Panel->openedFolder++;
		Panel->count = AdvFileManagerList(Panel->dir, &Panel->files);
	}
}

void AdvFileManagerFileAction(TCHAR filePath[])
{
	FILINFO fno;	
	memset(&fno, 0, sizeof(FILINFO));
	FILINFO *myInfo = &fno;
	f_stat(filePath, myInfo);

	int actions_idx = 0;
	while (true)
	{
		//DRAW GUI
		ConsoleInit();
		ConsoleSetTitle(myInfo->fname);
		wchar_t* beg;
		for (i = 0; i < FILE_ACTIONS; i++)
		{
			if (i == actions_idx) beg = strings[STR_CURSOR];
			else beg = strings[STR_NO_CURSOR];

			if (i == 0)print(L"%ls Launch as firm\n", beg);
			if (i == 1)print(L"%ls File info\n", beg);
			if (i == 2)print(L"%ls Something...\n", beg);
		}
		ConsoleShow();

		//APP CONTROLS
		uint32_t pad_state = InputWait();
		if (pad_state & BUTTON_DOWN)
		{
			if (actions_idx != FILE_ACTIONS - 1) actions_idx++; //MOVE DOWN
			else actions_idx = 0; //MOVE DOWN While at bottom -> go to top
		}
		else if (pad_state & BUTTON_UP)
		{
			if (actions_idx != 0) actions_idx--; //MOVE UP
			else actions_idx = FILE_ACTIONS - 1; //MOVE UP While at top -> go to bottom
		}
		else if (pad_state & BUTTON_A)
		{
			switch(actions_idx)
			{
				case 0:	
				FirmLoader(filePath);	
				return;

				case 1:			
				while(!(pad_state & BUTTON_B))
				{
					ConsoleInit();
					ConsoleSetTitle(L"File Info");
					print(L"Name: %ls\n", myInfo->fname);
					print(L"Size: %lu byte\n", myInfo->fsize);
					print(L"Timestamp: %u/%02u/%02u, %02u:%02u\n", (myInfo->fdate >> 9) + 1980, myInfo->fdate >> 5 & 15, myInfo->fdate & 31, myInfo->ftime >> 11, myInfo->ftime >> 5 & 63);
					ConsoleShow();
					pad_state = InputWait();
				}
				break;

				case 2:
				//Something....
				return;				
			}
		}
		else if (pad_state & BUTTON_B) break;	
	}
}

/*Advanced File Manager main code*/

void AdvFileManagerMain(){
	panel_t Panels[2];
	int currentPanel = 0;

	//Reset both panels
	for (i = 0; i < 2; i++)
	{
		Panels[i].pointer = 0;
		Panels[i].beginning = 0;
		Panels[i].openedFolder = 0;
		Panels[i].enabled = 0;
		wcscpy(Panels[i].dir, L"");		
		Panels[i].count = AdvFileManagerList(Panels[i].dir, &Panels[i].files);
	}
	Panels[0].enabled = 1;

	

	while (true)
	{
		//Background
		wchar_t str[_MAX_LFN];
		swprintf(str, _MAX_LFN, L"/rxTools/Theme/%u/FM.bin", cfgs[CFG_THEME].val.i);
		DrawSplash(screentmp, str);

		AdvFileManagerShow(&Panels[0], 0);
		AdvFileManagerShow(&Panels[1], 155);
		memcpy(BOT_SCREEN, screentmp, SCREEN_SIZE); //Trick to avoid screen flickering

		uint32_t pad_state = InputWait();
		if (pad_state & BUTTON_DOWN) AdvFileManagerNextSelection(&Panels[currentPanel]);
		else if (pad_state & BUTTON_UP) AdvFileManagerPrevSelection(&Panels[currentPanel]);
		else if (pad_state & BUTTON_A) AdvFileManagerSelect(&Panels[currentPanel]);
		else if (pad_state & BUTTON_B)
		{
			if (Panels[currentPanel].openedFolder == 0) break;
			else AdvFileManagerBack(&Panels[currentPanel]);
		}
		else if (pad_state & BUTTON_LEFT) currentPanel = 0;
		else if (pad_state & BUTTON_RIGHT)currentPanel = 1;

		//Change page
		if (Panels[currentPanel].pointer - Panels[currentPanel].beginning == 10)Panels[currentPanel].beginning += 10;
		if (Panels[currentPanel].pointer<Panels[currentPanel].beginning)Panels[currentPanel].beginning -= 10;

		//Set who's enabled
		if (!currentPanel) { Panels[0].enabled = 1; Panels[1].enabled = 0; }
		else { Panels[0].enabled = 0; Panels[1].enabled = 1; }
	}
}
