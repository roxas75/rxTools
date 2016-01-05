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

#ifndef ADVANCEDFILEMANAGER_H
#define ADVANCEDFILEMANAGER_H

#include <wchar.h>
#include "console.h"

#pragma once

typedef struct
{
	int pointer, beginning, openedFolder;
	size_t count;
	TCHAR **files;
	TCHAR dir[1000];
	int enabled; 
} panel_t;

void AdvFileManagerShow(panel_t* Panel, int x);
void AdvFileManagerNextSelection(panel_t* Panel);
void AdvFileManagerPrevSelection(panel_t* Panel);
void AdvFileManagerSelect(panel_t* Panel);
void AdvFileManagerBack(panel_t* Panel);
void AdvFileManagerMain();
#endif
