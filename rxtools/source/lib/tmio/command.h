/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2016 173210
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU General Public License Version 2, as described below:
 *
 * This file is free software: you may copy, redistribute and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 2 of the License, or (at your
 * option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 */

#ifndef COMMAND_H
#define COMMAND_H

enum {
	// Class 1
	MMC_IDLE = 0,
	MMC_SEND_OP_COND = 1,
	MMC_ALL_SEND_CID = 2,
	MMC_SET_RELATIVE_ADDR = 3,
	MMC_SWITCH = 6,
	MMC_SELECT_CARD = 7,
	MMC_SEND_CSD = 9,
	MMC_SEND_STATUS = 13,

	// Class 2
	MMC_SET_BLOCKLEN = 16,
	MMC_READ_BLOCK_MULTI = 18,

	// Class 4
	MMC_WRITE_BLOCK_MULTI = 25,

	// Class 8
	MMC_APP_CMD = 55
};

enum {
	// Class 0
	SD_SEND_IF_COND = 8,

	// Application command
	SD_APP_OP_COND = 41
};

#endif
