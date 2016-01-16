/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2014-2015, Normmatt, 173210
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

#ifndef TMIO_H
#define TMIO_H

#include <stdint.h>

enum tmio_dev_id {
	TMIO_DEV_SDMC = 0,
	TMIO_DEV_NAND = 1,

	TMIO_DEV_NUM
};

struct tmio_dev {
	uint32_t initarg;
	uint32_t isSDHC;
	uint32_t clk;
	uint32_t SDOPT;
	uint32_t total_size; //size in sectors of the device
	uint32_t res;
};

extern struct tmio_dev tmio_dev[TMIO_DEV_NUM];

void tmio_init(void);
uint32_t tmio_init_sdmc(void);
uint32_t tmio_init_nand(void);

uint32_t tmio_readsectors(enum tmio_dev_id target,
	uint32_t sector_no, uint32_t numsectors, uint8_t *out);

uint32_t tmio_writesectors(enum tmio_dev_id target,
	uint32_t sector_no, uint32_t numsectors, uint8_t *in);

#define TMIO_BBS 512

#endif
