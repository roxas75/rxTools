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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define rxmode_emu_label "RX3D"
#define rxmode_sys_label "Ver."

#include "common.h"

enum {
	CONF_NOKEYX = -1,
	CONF_NOFIRMBIN = -2,
	CONF_ERRNFIRM = -3,
	CONF_CANTOPENFILE = -4,
};

extern bool first_boot;

int InstallData(char* drive);
void InstallConfigData();

#endif
