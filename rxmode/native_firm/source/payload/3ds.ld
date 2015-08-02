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

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

/*0x080C3EE0*/
ENTRY(_start)

SECTIONS
{
  . = 0x0801A4C0;
  start_addr = .;
  .text.start : { *(.text.start) }
  .text       : { *(.text) *(.text*) }
  .rodata     : { *(.rodata) *(.rodata*) }
  .data       : { *(.data) *(.data*) }
  .bss        : { *(.bss) *(.bss*) }
  /*. = ALIGN(32);*/
  /*.stack : {
    stack_start = .;
    . += 0x40;
    . = ALIGN(32);
    stack_end = .;
  }*/
  total_size = . - start_addr;
}
