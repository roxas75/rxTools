/*
 * Copyright (C) 2015-2016 The PASTA Team
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

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(toolStart)

SECTIONS
{
	. = 0x23E00000;

	.text :
	{
		KEEP(*(.text.cstart))
		*(.text*)
	}

	.rodata : { *(.rodata*) }
	.data : { *(.data*) }
	.bss : { *(.bss*) }

	. = ALIGN(4);
	__end__ = ABSOLUTE(.);
}
