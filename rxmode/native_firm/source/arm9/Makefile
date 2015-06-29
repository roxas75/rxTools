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
