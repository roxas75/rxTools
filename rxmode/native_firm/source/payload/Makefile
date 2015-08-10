# Copyright (C) 2015 The PASTA Team
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

ifndef LDSCRIPT
$(error "Please set LDSCRIPT")
endif

include $(DEVKITARM)/ds_rules

OD=arm-none-eabi-objdump
PATCHES= arm9/EmuNAND.s arm9/CreateThread.s arm9/myThread/FS.S
CFLAGS=-c -mcpu=arm946e-s -march=armv5te -mlittle-endian -fshort-wchar -fno-zero-initialized-in-bss -Os
SFLAGS=-c -mcpu=arm946e-s -march=armv5te -mlittle-endian -fshort-wchar
# No linker flags yet, but here if you need it
LDFLAGS=

all: payload.elf

payload.elf:
	$(CC) -g -Wall arm9/myThread/myThread.c arm9/myThread/handlers.s $(CFLAGS) -Iinclude
	$(CC) -c arm11/hookswi.s
	$(CC) -g $(PATCHES) -I arm9/myThread $(SFLAGS)
	$(CC) -nostdlib -T $(LDSCRIPT) EmuNAND.o CreateThread.o myThread.o handlers.o hookswi.o FS.o -o $@
	rm -f *.o
