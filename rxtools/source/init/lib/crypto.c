/*
 * Copyright (C) 2015 The PASTA Team
 * Original version by megazig
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

#include <stddef.h>
#include <stdint.h>
#include <lib/crypto.h>

static void writeFifo(uint32_t value)
{
    *REG_AESWRFIFO = value;
}

static uint32_t readFifo(void)
{
    return *REG_AESRDFIFO;
}

static uint32_t getWriteCount()
{
    return *REG_AESCNT & 0x1F;
}

static uint32_t getReadCount()
{
    return (*REG_AESCNT >> 5) & 0x1F;
}

static void fifo(const void* inbuf, void* outbuf, size_t blocks)
{
    const size_t blockWords = AES_BLOCK_SIZE / 4;
    const uint32_t *in;
    uint32_t *out;
    uint16_t bufBlocks;
    int i;

    in = inbuf;
    out = outbuf;
    while (blocks > 0)
    {
        bufBlocks = 8 - getWriteCount();
        if (bufBlocks > blocks)
            bufBlocks = blocks;

        for (i = 0; i < bufBlocks * blockWords; i++)
        {
            writeFifo(*in);
            in++;
        }

        while (getReadCount() < bufBlocks);

        for (i = 0; i < bufBlocks * blockWords; i++)
        {
            *out = readFifo();
            out++;
        }

        blocks -= bufBlocks;
    }
}

static void start(uint32_t mode)
{
    *REG_AESCNT = mode |
                  AES_CNT_START |
                  AES_CNT_INPUT_ORDER |
                  AES_CNT_OUTPUT_ORDER |
                  AES_CNT_INPUT_ENDIAN |
                  AES_CNT_OUTPUT_ENDIAN |
                  AES_CNT_FLUSH_READ |
                  AES_CNT_FLUSH_WRITE;
}

void aesSetKeyX(uint8_t keyslot, void* keyx)
{
    uint32_t * _keyx = (uint32_t*)keyx;
    *REG_AESKEYCNT = (*REG_AESKEYCNT >> 6 << 6) | keyslot| 0x80;
    *REG_AESKEYXFIFO = _keyx[0];
    *REG_AESKEYXFIFO = _keyx[1];
    *REG_AESKEYXFIFO = _keyx[2];
    *REG_AESKEYXFIFO = _keyx[3];
}

void aesSetKey(uint32_t keyno, int value, void* key)
{
    volatile uint32_t* aes_regs[] =
    {
        (volatile uint32_t*)0x19009060,
        (volatile uint32_t*)0x10009090,
        (volatile uint32_t*)0x100090C0,
        (volatile uint32_t*)0x100090F0
    };
    uint32_t * _key = (uint32_t*)key;
    *REG_AESCNT = (*REG_AESCNT & ~(AES_CNT_INPUT_ENDIAN|AES_CNT_INPUT_ORDER)) | (value << 23);
    if (keyno > 3)
    {
        if (keyno > 0x3F)
            return;
        *REG_AESKEYCNT = (*REG_AESKEYCNT >> 6 << 6) | (uint8_t)keyno | 0x80;
        *REG_AESKEYFIFO = _key[0];
        *REG_AESKEYFIFO = _key[1];
        *REG_AESKEYFIFO = _key[2];
        *REG_AESKEYFIFO = _key[3];
    }
    else
    {
        volatile uint32_t* aes_reg = aes_regs[keyno];
        if (value & 0x4)
        {
            aes_reg[0] = _key[3];
            aes_reg[1] = _key[2];
            aes_reg[2] = _key[1];
            aes_reg[3] = _key[0];
        }
        else
        {
            aes_reg[0] = _key[0];
            aes_reg[1] = _key[1];
            aes_reg[2] = _key[2];
            aes_reg[3] = _key[3];
        }
    }
}

void aesSelKey(uint32_t keyno)
{
    if (keyno > 0x3F)
        return;
    *REG_AESKEYSEL = keyno;
    *REG_AESCNT    = *REG_AESCNT | 0x04000000; /* mystery bit */
}

void aesSetCtr(void* iv)
{
    uint32_t * _iv = (uint32_t*)iv;
    *(REG_AESCTR + 0) = _iv[3];
    *(REG_AESCTR + 1) = _iv[2];
    *(REG_AESCTR + 2) = _iv[1];
    *(REG_AESCTR + 3) = _iv[0];
}

void aesAddCtr(void* ctr, uint32_t carry)
{
    uint32_t counter[4];
    uint8_t *outctr = (uint8_t *) ctr;
    uint32_t sum;
    int32_t i;

    for(i=0; i<4; i++) {
        counter[i] = (outctr[i*4+0]<<24) | (outctr[i*4+1]<<16) | (outctr[i*4+2]<<8) | (outctr[i*4+3]<<0);
    }

    for(i=3; i>=0; i--)
    {
        sum = counter[i] + carry;
        if (sum < counter[i]) {
            carry = 1;
        }
        else {
            carry = 0;
        }
        counter[i] = sum;
    }

    for(i=0; i<4; i++)
    {
        outctr[i*4+0] = counter[i]>>24;
        outctr[i*4+1] = counter[i]>>16;
        outctr[i*4+2] = counter[i]>>8;
        outctr[i*4+3] = counter[i]>>0;
    }
}

void aesDecrypt(void *dst, const void *src, uint16_t blocks, uint32_t mode)
{
    *REG_AESCNT = 0;
    *REG_AESBLKCNT = blocks << 16;
    start(mode);
    fifo(src, dst, blocks);
}

void aesCtr(void *dst, const void *src, size_t n, void *ctr)
{
    *REG_AESBLKCNT = UINT16_MAX << 16;
    while (n >= UINT16_MAX) {
        aesSetCtr(ctr);
        start(AES_CTR_MODE);
        fifo(src, dst, UINT16_MAX);
        src = (void *)((uintptr_t)src + UINT16_MAX * AES_BLOCK_SIZE);
        dst = (void *)((uintptr_t)dst + UINT16_MAX * AES_BLOCK_SIZE);
        n -= UINT16_MAX;
        aesAddCtr(ctr, UINT16_MAX);
    }

    *REG_AESBLKCNT = n << 16;
    aesSetCtr(ctr);
    start(AES_CTR_MODE);
    fifo(src, dst, n);
}
