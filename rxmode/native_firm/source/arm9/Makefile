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

include $(DEVKITARM)/ds_rules

OD=arm-none-eabi-objdump
PATCHES= source/EmuNAND.s source/CreateThread.s source/myThread/FS.S
CFLAGS=-c -mcpu=arm946e-s -march=armv5te -mlittle-endian -fshort-wchar -std=c99
SFLAGS=-c -mcpu=arm946e-s -march=armv5te -mlittle-endian -fshort-wchar
# No linker flags yet, but here if you need it
LDFLAGS=

all:
	$(CC) -g -Wall source/myThread/myThread.c source/myThread/lib.c $(CFLAGS)
	$(CC) -g $(PATCHES) -I source/myThread $(SFLAGS)
	$(CC) -nostdlib -T 3ds.ld EmuNAND.o CreateThread.o myThread.o lib.o FS.o
	$(OBJCOPY) -O binary a.out arm9.bin
#	$(OD) -t a.out
	rm -f *.o *.out
	mv arm9.bin ../../build/0801a4c0.bin
