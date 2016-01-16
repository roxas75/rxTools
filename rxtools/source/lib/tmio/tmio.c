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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include "command.h"
#include "tmio.h"
#include "tmio_hardware.h"

#define DATA32_SUPPORT

void waitcycles(uint32_t val);

struct tmio_dev tmio_dev[TMIO_DEV_NUM] = {
	{ .total_size = 0 },
	{ .total_size = 0 }
};

_Static_assert(TMIO_DEV_NUM == 2,
	"TMIO device numer doesn't accord with the driver context.");

static int waitDataend = 0;

static uint16_t tmio_read16(enum tmio_regs reg) {
	return *(volatile uint16_t*)(TMIO_BASE + reg);
}

static void tmio_write16(enum tmio_regs reg, uint16_t val) {
	*(volatile uint16_t*)(TMIO_BASE + reg) = val;
}

static uint32_t tmio_read32(enum tmio_regs reg) {
	return *(volatile uint32_t*)(TMIO_BASE + reg);
}

static void tmio_write32(enum tmio_regs reg, uint32_t val) {
	*(volatile uint32_t*)(TMIO_BASE + reg) = val;
}

static void tmio_mask16(enum tmio_regs reg, const uint16_t clear, const uint16_t set) {
	uint16_t val = tmio_read16(reg);
	val &= ~clear;
	val |= set;
	tmio_write16(reg, val);
}

static void setckl(uint32_t data)
{
	tmio_mask16(REG_SDCLKCTL,0x100,0);
	tmio_mask16(REG_SDCLKCTL,0x2FF,data&0x2FF);
	tmio_mask16(REG_SDCLKCTL,0x0,0x100);
}

static void tmio_wfi()
{
	// Acknowledge the previous TMIO IRQ
	*(volatile uint32_t *)0x10001004 = 0x00010000;

	// Wait for interrupt
	__asm__ volatile ("mcr p15, 0, %0, c7, c0, 4" :: "r"(0));
}

static void inittarget(enum tmio_dev_id target)
{
	uint32_t status;

	if(waitDataend)
	{
		tmio_write32(REG_SDIRMASK, ~(TMIO_STAT_DATAEND | TMIO_MASK_GW));
		do
		{
			tmio_wfi();
			tmio_write32(REG_SDSTATUS, ~TMIO_STAT_DATAEND);

			status = tmio_read32(REG_SDSTATUS);
			if((status & TMIO_MASK_GW))
				break;
		}
		while ((status & TMIO_STAT_CMD_BUSY));

		waitDataend = 0;
	}

	tmio_mask16(REG_SDPORTSEL,0x3,target);
	setckl(tmio_dev[target].clk);
	if(tmio_dev[target].SDOPT == 0)
	{
		tmio_mask16(REG_SDOPT,0,0x8000);
	}
	else
	{
		tmio_mask16(REG_SDOPT,0x8000,0);
	}
}

static uint32_t tmio_wait_respend()
{
	uint32_t status, error;

	tmio_write32(REG_SDIRMASK, ~(TMIO_STAT_CMDRESPEND | TMIO_MASK_GW));
	do
	{
		tmio_wfi();
		status = tmio_read32(REG_SDSTATUS);
		error = status & TMIO_MASK_GW;
		if(error)
			return error;
	}
	while(!(status & TMIO_STAT_CMDRESPEND));

	return 0;
}

static uint32_t tmio_send_command(uint16_t cmd, uint32_t args, int cap_prev_error)
{
	uint32_t r;

	if ((tmio_read32(REG_SDSTATUS) & TMIO_STAT_CMD_BUSY))
	{
		r = tmio_wait_respend();
		if(r && cap_prev_error)
			return r;
	}

	tmio_write32(REG_SDSTATUS,0);
	tmio_write32(REG_SDCMDARG,args);
	tmio_write16(REG_SDCMD,cmd);

	return 0;
}

uint32_t tmio_readsectors(enum tmio_dev_id target,
	uint32_t sector_no, uint32_t numsectors, uint8_t *out)
{
	uint32_t error, mask;

	if(tmio_dev[target].isSDHC == 0) sector_no <<= 9;
	inittarget(target);
	tmio_write16(REG_SDSTOP,0x100);
#ifdef DATA32_SUPPORT
	tmio_write16(REG_SDBLKCOUNT32,numsectors);
	tmio_write16(REG_SDBLKLEN32,TMIO_BBS);
#endif
	tmio_write16(REG_SDBLKCOUNT,numsectors);

	mask = TMIO_MASK_GW;
#ifdef DATA32_SUPPORT
	tmio_write16(REG_DATACTL32,TMIO32_ENABLE | TMIO32_IRQ_RXRDY);
#else
	mask |= TMIO_STAT_RXRDY;
#endif

	tmio_write32(REG_SDIRMASK,~mask);
	tmio_send_command(MMC_READ_BLOCK_MULTI
		| TMIO_CMD_RESP_R1 | TMIO_CMD_DATA_PRESENT
		| TMIO_CMD_TRANSFER_READ | TMIO_CMD_TRANSFER_MULTI,
		sector_no, 0);

	uint16_t *dataPtr = (uint16_t*)out;
	uint32_t *dataPtr32 = (uint32_t*)out;
	int useBuf32 = 0 == (3 & ((uint32_t)dataPtr));

	while(numsectors > 0)
	{
		tmio_wfi();

		error = tmio_read32(REG_SDSTATUS) & TMIO_MASK_GW;
		if(error)
			return error;

#ifdef DATA32_SUPPORT
		if(!(tmio_read16(REG_DATACTL32) & TMIO32_STAT_RXRDY))
			continue;
#endif

		#ifdef DATA32_SUPPORT
		if(useBuf32)
		{
			for(int i = 0; i<TMIO_BBS; i+=4)
			{
				*dataPtr32++ = tmio_read32(REG_SDFIFO32);
			}
		}
		else
		{
		#endif
			for(int i = 0; i<TMIO_BBS; i+=2)
			{
				*dataPtr++ = tmio_read16(REG_SDFIFO);
			}
		#ifdef DATA32_SUPPORT
		}
		#endif
		numsectors--;
	}

	return 0;
}

uint32_t tmio_writesectors(enum tmio_dev_id target,
	uint32_t sector_no, uint32_t numsectors, uint8_t *in)
{
	uint32_t error, mask;

	if(tmio_dev[target].isSDHC == 0) sector_no <<= 9;
	inittarget(target);
	tmio_write16(REG_SDSTOP,0x100);
#ifdef DATA32_SUPPORT
	tmio_write16(REG_SDBLKCOUNT32,numsectors);
	tmio_write16(REG_SDBLKLEN32,TMIO_BBS);
#endif
	tmio_write16(REG_SDBLKCOUNT,numsectors);

	mask = TMIO_MASK_GW;
#ifdef DATA32_SUPPORT
	tmio_write16(REG_DATACTL32,TMIO32_ENABLE | TMIO32_IRQ_TXRQ);
#else
	mask |= TMIO_STAT_RXRDY;
#endif
	tmio_write32(REG_SDIRMASK,~mask);

	tmio_send_command(MMC_WRITE_BLOCK_MULTI
		| TMIO_CMD_RESP_R1 | TMIO_CMD_DATA_PRESENT
		| TMIO_CMD_TRANSFER_MULTI,
		sector_no, 0);

#ifdef DATA32_SUPPORT
	uint32_t *dataPtr32 = (uint32_t*)in;
#else
	uint16_t *dataPtr = (uint16_t*)in;
#endif

	while(numsectors > 0)
	{
		tmio_wfi();

		error = tmio_read32(REG_SDSTATUS) & TMIO_MASK_GW;
		if(error)
			return error;

#ifdef DATA32_SUPPORT
		if((tmio_read16(REG_DATACTL32) & TMIO32_STAT_BUSY))
			continue;
#endif

		#ifdef DATA32_SUPPORT
		for(int i = 0; i<TMIO_BBS; i+=4)
		{
			tmio_write32(REG_SDFIFO32,*dataPtr32++);
		}
		#else
		for(int i = 0; i<TMIO_BBS; i+=2)
		{
			tmio_write16(REG_SDFIFO,*dataPtr++);
		}
		#endif
		numsectors--;
	}

	waitDataend = 1;
	return 0;
}

static uint32_t calcSDSize(uint8_t* csd, int type)
{
  uint32_t result=0;
  if(type == -1) type = csd[14] >> 6;
  switch(type)
  {
    case 0:
      {
        uint32_t block_len=csd[9]&0xf;
        block_len=1<<block_len;
        uint32_t mult=(csd[4]>>7)|((csd[5]&3)<<1);
        mult=1<<(mult+2);
        result=csd[8]&3;
        result=(result<<8)|csd[7];
        result=(result<<2)|(csd[6]>>6);
        result=(result+1)*mult*block_len/512;
      }
      break;
    case 1:
      result=csd[7]&0x3f;
      result=(result<<8)|csd[6];
      result=(result<<8)|csd[5];
      result=(result+1)*1024;
      break;
  }
  return result;
}

void tmio_init()
{
	tmio_mask16(REG_DATACTL32, 0x0800, 0x0000);
	tmio_mask16(REG_DATACTL32, 0x1000, 0x0000);
	tmio_mask16(REG_DATACTL32, 0x0000, 0x0402);
	tmio_mask16(REG_DATACTL, 0x2200, 0x0002);
#ifdef DATA32_SUPPORT
	tmio_mask16(REG_DATACTL, 0x0020, 0x0000);
	tmio_write16(REG_SDBLKLEN32, TMIO_BBS);
#else
	tmio_mask16(REG_DATACTL32, 0x0020, 0x0000);
	tmio_mask16(REG_DATACTL, 0x0020, 0x0000);
	tmio_write16(REG_SDBLKLEN32, 0);
#endif
	tmio_write16(REG_SDBLKCOUNT32, 1);
	tmio_mask16(REG_SDRESET, 0x0001, 0x0000);
	tmio_mask16(REG_SDRESET, 0x0000, 0x0001);
	tmio_write32(REG_SDIRMASK, ~TMIO_MASK_ALL);
	tmio_mask16(0xFC, 0x0000, 0x00DB);
	tmio_mask16(0xFE, 0x0000, 0x00DB);
	tmio_mask16(REG_SDPORTSEL, 0x0002, 0x0000);
#ifdef DATA32_SUPPORT
	tmio_write16(REG_SDCLKCTL, 0x0020);
	tmio_write16(REG_SDOPT, 0x40EE);
#else
	tmio_write16(REG_SDCLKCTL, 0x0040);
	tmio_write16(REG_SDOPT, 0x40EB);
#endif
	tmio_mask16(REG_SDPORTSEL, 0x0002, 0x0000);
	tmio_write16(REG_SDBLKLEN, TMIO_BBS);
	tmio_write16(REG_SDSTOP, 0);
	
	inittarget(TMIO_DEV_SDMC);
}

uint32_t tmio_init_nand()
{
	uint32_t r;

	if (tmio_dev[TMIO_DEV_NAND].total_size > 0)
		return 0;

	//NAND
	tmio_dev[TMIO_DEV_NAND].isSDHC = 0;
	tmio_dev[TMIO_DEV_NAND].SDOPT = 0;
	tmio_dev[TMIO_DEV_NAND].res = 0;
	tmio_dev[TMIO_DEV_NAND].initarg = 1;
	tmio_dev[TMIO_DEV_NAND].clk = 0x80;

	inittarget(TMIO_DEV_NAND);
	waitcycles(0xF000);
	
	tmio_send_command(MMC_IDLE,0,0);
	
	do
	{
		do
			tmio_send_command(MMC_SEND_OP_COND | TMIO_CMD_RESP_R3,
				0x100000,0);
		while (tmio_wait_respend());
	}
	while((tmio_read32(REG_SDRESP0) & 0x80000000) == 0);
	
	tmio_send_command(MMC_ALL_SEND_CID | TMIO_CMD_RESP_R2,0x0,0);
	r = tmio_send_command(MMC_SET_RELATIVE_ADDR | TMIO_CMD_RESP_R1,
		tmio_dev[TMIO_DEV_NAND].initarg << 0x10,1);
	if(r)
		return r;

	r = tmio_send_command(MMC_SEND_CSD | TMIO_CMD_RESP_R2,
		tmio_dev[TMIO_DEV_NAND].initarg << 0x10,1);
	if(r)
		return r;

	r = tmio_wait_respend();
	if(r)
		return r;
	
	tmio_dev[TMIO_DEV_NAND].total_size = calcSDSize((uint8_t*)TMIO_BASE + REG_SDRESP0,0);
	tmio_dev[TMIO_DEV_NAND].clk = 1;
	setckl(1);
	
	tmio_send_command(MMC_SELECT_CARD | TMIO_CMD_RESP_R1,
		tmio_dev[TMIO_DEV_NAND].initarg << 0x10,0);

	tmio_dev[TMIO_DEV_NAND].SDOPT = 1;

	r = tmio_send_command(MMC_SWITCH | TMIO_CMD_RESP_R1B,0x3B70100,1);
	if(r)
		return r;

	r = tmio_send_command(MMC_SWITCH | TMIO_CMD_RESP_R1B,0x3B90100,1);
	if(r)
		return r;

	r = tmio_send_command(MMC_SEND_STATUS | TMIO_CMD_RESP_R1,
		tmio_dev[TMIO_DEV_NAND].initarg << 0x10,1);
	if(r)
		return r;

	r = tmio_send_command(MMC_SET_BLOCKLEN | TMIO_CMD_RESP_R1,TMIO_BBS,1);
	if(r)
		return r;

	tmio_dev[TMIO_DEV_NAND].clk |= 0x200;
	
	inittarget(TMIO_DEV_SDMC);
	
	return 0;
}

uint32_t tmio_init_sdmc()
{
	uint32_t resp;
	uint32_t r;

	if (tmio_dev[TMIO_DEV_SDMC].total_size > 0)
		return 0;

	//SD
	tmio_dev[TMIO_DEV_SDMC].isSDHC = 0;
	tmio_dev[TMIO_DEV_SDMC].SDOPT = 0;
	tmio_dev[TMIO_DEV_SDMC].res = 0;
	tmio_dev[TMIO_DEV_SDMC].initarg = 0;
	tmio_dev[TMIO_DEV_SDMC].clk = 0x80;

	inittarget(TMIO_DEV_SDMC);
	waitcycles(0xF000);
	tmio_send_command(MMC_IDLE,0,0);

	r = tmio_send_command(SD_SEND_IF_COND | TMIO_CMD_RESP_R1,0x1AA,1);
	if(r)
		return r;

	uint32_t temp = tmio_wait_respend() ? 0 : 0x1 << 0x1E;

	uint32_t temp2 = 0;
	do
	{
		while(1)
		{
			tmio_send_command(MMC_APP_CMD | TMIO_CMD_RESP_R1,
				tmio_dev[TMIO_DEV_SDMC].initarg << 0x10,0);
			temp2 = 1;
			if(tmio_send_command(SD_APP_OP_COND
				| TMIO_CMD_APP | TMIO_CMD_RESP_R3,
				0x00FF8000 | temp,1))
				continue;
			if(!tmio_wait_respend())
				break;
		}

		resp = tmio_read32(REG_SDRESP0);
	}
	while((resp & 0x80000000) == 0);

	if(!((resp >> 30) & 1) || !temp)
		temp2 = 0;
	
	tmio_dev[TMIO_DEV_SDMC].isSDHC = temp2;
	
	tmio_send_command(MMC_ALL_SEND_CID | TMIO_CMD_RESP_R2,0,0);
	
	r = tmio_send_command(MMC_SET_RELATIVE_ADDR | TMIO_CMD_RESP_R1,0,1);
	if(r)
		return r;

	r = tmio_wait_respend();
	if(r)
		return r;

	tmio_dev[TMIO_DEV_SDMC].initarg = tmio_read32(REG_SDRESP0) >> 0x10;
	tmio_send_command(MMC_SEND_CSD | TMIO_CMD_RESP_R2,
		tmio_dev[TMIO_DEV_SDMC].initarg << 0x10,0);
	r = tmio_wait_respend();
	if(r)
		return r;

	tmio_dev[TMIO_DEV_SDMC].total_size = calcSDSize((uint8_t*)TMIO_BASE + REG_SDRESP0,-1);
	tmio_dev[TMIO_DEV_SDMC].clk = 1;
	setckl(1);
	
	tmio_send_command(MMC_SELECT_CARD | TMIO_CMD_RESP_R1B,
		tmio_dev[TMIO_DEV_SDMC].initarg << 0x10,0);
	r = tmio_send_command(MMC_APP_CMD | TMIO_CMD_RESP_R1,
		tmio_dev[TMIO_DEV_SDMC].initarg << 0x10,1);
	if(r)
		return r;
	
	tmio_dev[TMIO_DEV_SDMC].SDOPT = 1;
	r = tmio_send_command(MMC_SWITCH | TMIO_CMD_APP | TMIO_CMD_RESP_R1,
		0x2,1);
	if(r)
		return r;

	r = tmio_send_command(MMC_SEND_STATUS | TMIO_CMD_RESP_R1,
		tmio_dev[TMIO_DEV_SDMC].initarg << 0x10,1);
	if(r)
		return r;
	
	r = tmio_send_command(MMC_SET_BLOCKLEN | TMIO_CMD_RESP_R1,TMIO_BBS,1);
	if(r)
		return r;

	tmio_dev[TMIO_DEV_SDMC].clk |= 0x200;
	
	return 0;
}

uint32_t tmio_wrprotected(enum tmio_dev_id target)
{
	inittarget(target);
	return tmio_read32(REG_SDSTATUS) & TMIO_STAT_WRPROTECT;
}
