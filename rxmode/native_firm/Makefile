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

#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

ifndef BUILD
$(error "Please set BUILD")
endif

include $(DEVKITARM)/base_rules

TARGET	:= native_firm
SOURCE	:= source
LD	:= arm-none-eabi-ld

CFLAGS	:= -Os -mcpu=arm946e-s -march=armv5te -mlittle-endian -fshort-wchar -fno-zero-initialized-in-bss $(INCDIR)

PATCHES := $(BUILD)/payload/payload.elf $(BUILD)/k9_fix_flags.elf	\
	$(BUILD)/p9_emunand_hook_1.elf $(BUILD)/p9_emunand_hook_2.elf	\
	$(BUILD)/p9_mythread_hook_1.elf $(BUILD)/p9_mythread_hook_2.elf	\
	$(BUILD)/p9_signatures_spoof_1.elf $(BUILD)/p9_signatures_spoof_2.elf

PAYLOAD_OBJS	:= $(BUILD)/payload/arm9/EmuNAND.o	\
	$(BUILD)/payload/arm9/CreateThread.o $(BUILD)/payload/arm9/vars.o	\
	$(BUILD)/payload/arm9/myThread/myThread.o

ifdef PLATFORM_KTR
LDSCRIPT	:= ktr.ld
CFLAGS	+= -DPLATFORM_KTR
else
LDSCRIPT	:= ctr.ld

PATCHES	+= $(BUILD)/p9_keyx_spoof.elf	\
	$(BUILD)/p9_reboothax_1.elf $(BUILD)/p9_reboothax_2.elf

PAYLOAD_OBJS	+= $(BUILD)/payload/arm9/myThread/FS.o	\
	$(BUILD)/payload/arm9/myThread/handlers.o	\
	$(BUILD)/payload/arm11/hookswi.o
endif

.PHONY: clean all

all: $(BUILD)/$(TARGET).elf

$(BUILD):
	@[ -d $@ ] || mkdir -p $@

$(BUILD)/payload:
	@[ -d $@ ] || mkdir -p $@

$(BUILD)/payload/arm9:
	@[ -d $@ ] || mkdir -p $@

$(BUILD)/payload/arm9/myThread:
	@[ -d $@ ] || mkdir -p $@

$(BUILD)/payload/arm11:
	@[ -d $@ ] || mkdir -p $@

%.elf: %_unstrip.elf
	$(STRIP) $< -o $@

$(BUILD)/$(TARGET)_unstrip.elf: $(PATCHES)
	$(LD) -i -T$(LDSCRIPT) $^ -o $@

$(BUILD)/payload/payload.elf: $(PAYLOAD_OBJS) $(BUILD)/payload
	$(LD) -T$(LDSCRIPT) $(PAYLOAD_OBJS) -o $@

$(BUILD)/%.elf: $(BUILD)/%.o $(BUILD)/payload/payload.elf
	$(LD) -s -R $(BUILD)/payload/payload.elf -T$(LDSCRIPT) $< -o $@

$(BUILD)/payload/arm9/myThread/%.o: $(SOURCE)/payload/arm9/myThread/%.c $(BUILD)/payload/arm9/myThread
	$(CC) $(CFLAGS) -I$(SOURCE)/payload/include $< -c -o $@

$(BUILD)/payload/arm9/myThread/%.o: $(SOURCE)/payload/arm9/myThread/%.s $(BUILD)/payload/arm9/myThread
	$(AS) $< -o $@

$(BUILD)/payload/arm9/%.o: $(SOURCE)/payload/arm9/%.S $(BUILD)/payload/arm9
	$(CC) $(CFLAGS) -I$(SOURCE)/payload/include $< -c -o $@

$(BUILD)/payload/arm11/%.o: $(SOURCE)/payload/arm11/%.S $(BUILD)/payload/arm11
	$(CC) $(CFLAGS) -I$(SOURCE)/payload/include $< -c -o $@

$(BUILD)/%.o: $(SOURCE)/%.S $(BUILD)
	$(CC) $(CFLAGS) $< -c -o $@

$(BUILD)/%.o: $(SOURCE)/%.s $(BUILD)
	$(AS) $< -o $@
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD)
