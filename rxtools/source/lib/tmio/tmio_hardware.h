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

#ifndef TMIO_HARDWARE_H
#define TMIO_HARDWARE_H

#define TMIO_BASE				0x10006000

enum tmio_regs {
	REG_SDCMD				= 0x00,
	REG_SDPORTSEL			= 0x02,
	REG_SDCMDARG			= 0x04,
	REG_SDSTOP				= 0x08,
	REG_SDBLKCOUNT			= 0x0a,

	REG_SDRESP0				= 0x0c,
	REG_SDRESP1				= 0x10,
	REG_SDRESP2				= 0x14,
	REG_SDRESP3				= 0x18,

	REG_SDSTATUS			= 0x1c,

	REG_SDIRMASK			= 0x20,
	REG_SDCLKCTL			= 0x24 ,

	REG_SDBLKLEN			= 0x26,
	REG_SDOPT				= 0x28,
	REG_SDFIFO				= 0x30,

	REG_DATACTL 			= 0xd8,
	REG_SDRESET				= 0xe0,
	REG_SDPROTECTED			= 0xf6, //bit 0 determines if sd is protected or not?

	REG_DATACTL32 			= 0x100,
	REG_SDBLKLEN32			= 0x104,
	REG_SDBLKCOUNT32		= 0x108,
	REG_SDFIFO32			= 0x10C,

	REG_CLK_AND_WAIT_CTL	= 0x138,
	REG_RESET_SDIO			= 0x1e0
};

#define TMIO_STAT_CMDRESPEND    0x00000001
#define TMIO_STAT_DATAEND       0x00000004
#define TMIO_STAT_CARD_REMOVE   0x00000008
#define TMIO_STAT_CARD_INSERT   0x00000010
#define TMIO_STAT_SIGSTATE      0x00000020
#define TMIO_STAT_WRPROTECT     0x00000080
#define TMIO_STAT_CARD_REMOVE_A 0x00000100
#define TMIO_STAT_CARD_INSERT_A 0x00000200
#define TMIO_STAT_SIGSTATE_A    0x00000400
#define TMIO_STAT_CMD_IDX_ERR   0x00010000
#define TMIO_STAT_CRCFAIL       0x00020000
#define TMIO_STAT_STOPBIT_ERR   0x00040000
#define TMIO_STAT_DATATIMEOUT   0x00080000
#define TMIO_STAT_RXOVERFLOW    0x00100000
#define TMIO_STAT_TXUNDERRUN    0x00200000
#define TMIO_STAT_CMDTIMEOUT    0x00400000
#define TMIO_STAT_RXRDY         0x01000000
#define TMIO_STAT_TXRQ          0x02000000
#define TMIO_STAT_ILL_FUNC      0x20000000
#define TMIO_STAT_CMD_BUSY      0x40000000
#define TMIO_STAT_ILL_ACCESS    0x80000000

#define TMIO_MASK_ALL           0x837f031d

#define TMIO_MASK_GW            (TMIO_STAT_ILL_ACCESS | TMIO_STAT_CMDTIMEOUT | TMIO_STAT_TXUNDERRUN | TMIO_STAT_RXOVERFLOW | \
								 TMIO_STAT_DATATIMEOUT | TMIO_STAT_STOPBIT_ERR | TMIO_STAT_CRCFAIL | TMIO_STAT_CMD_IDX_ERR)

#define TMIO_MASK_READOP  (TMIO_STAT_RXRDY | TMIO_STAT_DATAEND)
#define TMIO_MASK_WRITEOP (TMIO_STAT_TXRQ | TMIO_STAT_DATAEND)

enum {
	TMIO32_ENABLE		= 0x0002,
	TMIO32_STAT_RXRDY	= 0x0100,
	TMIO32_STAT_BUSY	= 0x0200,
	TMIO32_IRQ_RXRDY	= 0x0800,
	TMIO32_IRQ_TXRQ		= 0x1000
};

#endif
