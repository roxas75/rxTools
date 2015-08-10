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

include $(DEVKITARM)/base_rules

TARGET 		:=  native_firm
BUILD		:=	build
SOURCE		:=	source
LDSCRIPT	:=	script.ld
LD		:=	arm-none-eabi-ld

.PHONY: clean all

all: $(TARGET).elf

$(BUILD):
	@[ -d $@ ] || mkdir -p $@

$(BUILD)/payload/arm9:
	@[ -d $@ ] || mkdir -p $@

$(BUILD)/payload/arm9/myThread:
	@[ -d $@ ] || mkdir -p $@

$(BUILD)/payload/arm11:
	@[ -d $@ ] || mkdir -p $@

%.elf: %_unstrip.elf
	$(STRIP) $< -o $@

$(TARGET)_unstrip.elf: $(SOURCE)/payload/payload.elf	\
	$(BUILD)/k9_fix_flags.elf $(BUILD)/p9_keyx_spoof.elf	\
	$(BUILD)/p9_emunand_hook_1.elf $(BUILD)/p9_emunand_hook_2.elf	\
	$(BUILD)/p9_mythread_hook_1.elf $(BUILD)/p9_mythread_hook_2.elf	\
	$(BUILD)/p9_reboothax_1.elf $(BUILD)/p9_reboothax_2.elf	\
	$(BUILD)/p9_signatures_spoof_1.elf $(BUILD)/p9_signatures_spoof_2.elf
	$(LD) -i -T$(LDSCRIPT) $^ -o $@

$(SOURCE)/payload/payload.elf: $(BUILD)/payload/arm9/EmuNAND.o	\
	$(BUILD)/payload/arm9/CreateThread.o	\
	$(BUILD)/payload/arm9/myThread/myThread.o	\
	$(BUILD)/payload/arm9/myThread/handlers.o	\
	$(BUILD)/payload/arm9/myThread/FS.o	\
	$(BUILD)/payload/arm11/hookswi.o
	$(LD) -T$(LDSCRIPT) $^ -o $@

$(BUILD)/%.elf: $(BUILD)/%.o $(SOURCE)/payload/payload.elf
	$(LD) -s -R $(SOURCE)/payload/payload.elf -T$(LDSCRIPT) $< -o $@

$(BUILD)/payload/arm9/myThread/%.o: $(SOURCE)/payload/arm9/myThread/%.c $(BUILD)/payload/arm9/myThread
	$(CC) -Os -mcpu=arm946e-s -march=armv5te -mlittle-endian -fshort-wchar -fno-zero-initialized-in-bss -I$(SOURCE)/payload/include $< -c -o $@

$(BUILD)/payload/arm9/myThread/%.o: $(SOURCE)/payload/arm9/myThread/%.s $(BUILD)/payload/arm9/myThread
	$(AS) $< -o $@

$(BUILD)/payload/arm9/%.o: $(SOURCE)/payload/arm9/%.s $(BUILD)/payload/arm9
	$(AS) $< -o $@

$(BUILD)/payload/arm11/%.o: $(SOURCE)/payload/arm11/%.s $(BUILD)/payload/arm11
	$(AS) $< -o $@

$(BUILD)/%.o: $(SOURCE)/%.s $(BUILD)
	$(AS) $< -o $@
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET)_unstrip.elf $(TARGET).elf source/payload/payload.elf
