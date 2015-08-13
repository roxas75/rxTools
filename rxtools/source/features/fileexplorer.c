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
#include "configuration.h"
#include "lang.h"
#include "fileexplorer.h"
#include "draw.h"
#include "hid.h"
#include "fs.h"

//---- GLOBAL VARIABLES ----
int pointer = 0;
char **files;
size_t count;
int i;
int beginning = 0;
char dir[1000] = "/";
int opened_folder = 0;


size_t file_list(const char *path, char ***ls) {
	size_t count = 0;
	DIR myDir;
	FILINFO curInfo;
	memset(&myDir, 0, sizeof(DIR));
	memset(&curInfo, 0, sizeof(FILINFO));
	FILINFO *myInfo = &curInfo;

	int res=f_opendir(&myDir, dir);
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
		(*ls)[count++] = strdup(myInfo->fname);
		f_readdir(&myDir, myInfo);
	}

	f_closedir(&myDir);
	return count;
}

void FileExplorerShow(){
	ConsoleInit();
	ConsoleSetTitle(strings[STR_DIRECTORY], dir);
	if (count != 0)
	{
		int n = 0;
		int divisions = count % 7 == 0 ? count / 7 : count / 7 + 1;
		int list[divisions];
		for (n = 0; n<divisions; n++)
		{
			if (n == divisions - 1 && count % 7 != 0)list[n] = count - (count / 7) * 7;
			else list[n] = 7;
		}

		int i = 0;
		for (i = beginning; i < beginning + list[(pointer / 7)]; i++) {
			print(L"%ls %s\n", i == pointer ? strings[STR_CURSOR] : strings[STR_NO_CURSOR], files[i]);
		}
	}
	ConsoleShow();
}

void FileExplorerNextSelection(){
	if (pointer != count - 1) pointer++;
}

void FileExplorerPrevSelection(){
	if (pointer != 0) pointer--;
}

void FileExplorerBack(){
	//go back
	if (opened_folder != 0)
	{
		int u;
		dir[strlen(dir) - 1] = 'f'; //just a casual letter
		for (u = strlen(dir); u >= 0; u--)
		{
			if (dir[u] == '/') {
				if(opened_folder==1) dir[u + 1] = '\0';
				else dir[u] = '\0';
				break;
			}
		}
		beginning = 0;
		pointer = 0;
		opened_folder--;
		count = file_list(dir, &files);
	}
}

/* This writes the path to p if it is a file.
It returns written bytes to p if it succeeded in opening file or directory.
Otherwise it returns a negative value */
int FileExplorerSelect(char *p, size_t n){
	//enter file/folder
	int u;
	int isafile = 0;
	for (u = 0; u < strlen(files[pointer]); u++)
	{
		if (files[pointer][u] == '.') isafile = 1;
	}
	if (isafile){
		//Open file
		return snprintf(p, n, "%s%s%s", dir,
			opened_folder == 0 ? "" : "/", files[pointer]) > n ?
			-1 : n;
	}
	else
	{
		//enter folder
		if (opened_folder != 0) strcat(dir, "/");
		strcat(dir, files[pointer]);
		beginning = 0;
		pointer = 0;
		opened_folder++;
		count = file_list(dir, &files);
	}
	return 0;
}

/*File explorer main code
Can be called from where you want and returns the selected file!
*/

int FileExplorerMain(char *p, size_t n){
	count = file_list(dir, &files);
	while (true)
	{
		FileExplorerShow();

		uint32_t pad_state = InputWait();
		if (pad_state & BUTTON_DOWN) FileExplorerNextSelection();
		else if (pad_state & BUTTON_UP) FileExplorerPrevSelection();
		else if (pad_state & BUTTON_A)
		{
			if (FileExplorerSelect(p, n) > 0)return 0;
		}
		else if (pad_state & BUTTON_B)
		{
			if (opened_folder == 0)return -1;
			else FileExplorerBack();
		}

		if (pointer - beginning == 7)beginning += 7;
		if (pointer<beginning)beginning -= 7;
	}
}
