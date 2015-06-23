#include "ncch.h"

unsigned int align(unsigned int offset, unsigned int alignment)
{
	unsigned int mask = ~(alignment-1);

	return (offset + (alignment-1)) & mask;
}

unsigned long align64(unsigned long offset, unsigned int alignment)
{
	unsigned long mask = ~(alignment-1);

	return (offset + (alignment-1)) & mask;
}

unsigned long getle64(const unsigned char* p)
{
	unsigned long n = p[0];

	n |= (unsigned long)p[1]<<8;
	n |= (unsigned long)p[2]<<16;
	n |= (unsigned long)p[3]<<24;
	n |= (unsigned long)p[4]<<32;
	n |= (unsigned long)p[5]<<40;
	n |= (unsigned long)p[6]<<48;
	n |= (unsigned long)p[7]<<56;
	return n;
}

unsigned long getbe64(const unsigned char* p)
{
	unsigned long n = 0;

	n |= (unsigned long)p[0]<<56;
	n |= (unsigned long)p[1]<<48;
	n |= (unsigned long)p[2]<<40;
	n |= (unsigned long)p[3]<<32;
	n |= (unsigned long)p[4]<<24;
	n |= (unsigned long)p[5]<<16;
	n |= (unsigned long)p[6]<<8;
	n |= (unsigned long)p[7]<<0;
	return n;
}

unsigned int getle32(const unsigned char* p)
{
	return (p[0]<<0) | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);
}

unsigned int getbe32(const unsigned char* p)
{
	return (p[0]<<24) | (p[1]<<16) | (p[2]<<8) | (p[3]<<0);
}

unsigned int getle16(const unsigned char* p)
{
	return (p[0]<<0) | (p[1]<<8);
}

unsigned int getbe16(const unsigned char* p)
{
	return (p[0]<<8) | (p[1]<<0);
}

void putle16(unsigned char* p, unsigned short n)
{
	p[0] = n;
	p[1] = n>>8;
}

void putle32(unsigned char* p, unsigned int n)
{
	p[0] = n;
	p[1] = n>>8;
	p[2] = n>>16;
	p[3] = n>>24;
}

void ncch_get_counter(ctr_ncchheader header, unsigned char counter[16], unsigned char type)
{
	unsigned int version = getle16(header.version);
	unsigned int mediaunitsize = 0x200;
	unsigned char* partitionid = header.partitionid;
	unsigned int i;
	unsigned int x = 0;

	for(i = 0; i < 16; i++) counter[i] = 0x00;

	if (version == 2 || version == 0)
	{
		for(i=0; i<8; i++)
			counter[i] = partitionid[7-i];
		counter[8] = type;
	}
	else if (version == 1)
	{
		if (type == NCCHTYPE_EXHEADER)
			x = 0x200;
		else if (type == NCCHTYPE_EXEFS)
			x = getle32(header.exefsoffset) * mediaunitsize;
		else if (type == NCCHTYPE_ROMFS)
			x = getle32(header.romfsoffset) * mediaunitsize;

		for(i=0; i<8; i++)
			counter[i] = partitionid[i];
		for(i=0; i<4; i++)
			counter[12+i] = x>>((3-i)*8);
	}
}