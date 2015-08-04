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

#ifndef FILEPACK_H
#define FILEPACK_H

enum{
	TITLE_KEY = 0,
	REBOOT,
	PATCH,
};

typedef struct{
	unsigned int off;
	unsigned int size;
	unsigned int hash;
	unsigned int edited;
	char name[16];
}PackEntry;

int LoadPack();
void SavePack();
void* GetFilePack(char* name);
PackEntry* GetEntryPack(int filenumber);

unsigned int HashGen(unsigned char* file, unsigned int size);
int CheckHash(unsigned char* file, unsigned int size, unsigned int hash);  //0 if wrong signature

#endif
