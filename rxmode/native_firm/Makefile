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
OBJCOPY	:= arm-none-eabi-objcopy

CFLAGS	:= -Os -fshort-wchar -fno-zero-initialized-in-bss $(INCDIR)

PATCHES := $(BUILD)/payload/payload.elf $(BUILD)/k9_fix_flags.elf	\
	$(BUILD)/p9_emunand_hook_1.elf $(BUILD)/p9_emunand_hook_2.elf	\
	$(BUILD)/p9_mythread_hook_1.elf $(BUILD)/p9_mythread_hook_2.elf	\
	$(BUILD)/p9_signatures_spoof_1.elf $(BUILD)/p9_signatures_spoof_2.elf	\
	$(BUILD)/k11_codeset.elf

PAYLOAD_OBJS	:= $(BUILD)/payload/arm9/EmuNAND.o	\
	$(BUILD)/payload/arm9/myThread/FS.o	\
	$(BUILD)/payload/arm9/CreateThread.o $(BUILD)/payload/arm9/vars.o	\
	$(BUILD)/payload/arm9/myThread/myThread.o

ifdef PLATFORM_KTR
LDSCRIPT	:= ktr.ld
CFLAGS	+= -DPLATFORM_KTR
PROCESS9_CODE_ADDR	:= 0x0801CD00
else
LDSCRIPT	:= ctr.ld
PROCESS9_CODE_ADDR	:= 0x0801CF00

PATCHES	+= $(BUILD)/p9_keyx_spoof.elf	\
	$(BUILD)/p9_reboothax_1.elf $(BUILD)/p9_reboothax_2.elf

PAYLOAD_OBJS	+= 	\
	$(BUILD)/payload/arm9/myThread/handlers.o	\
	$(BUILD)/payload/arm11/hookswi.o
endif

$(foreach f,$(BUILD) $(PATCHES) $(PAYLOAD_OBJS),$(eval $f : | $(dir $f)D))

.PHONY: clean all

all: $(BUILD)/$(TARGET).elf

%/D:
	mkdir -p $@

%.elf: %_unadjust.elf
	$(OBJCOPY) -S --change-section-address .patch.p9.*+$$(($(PROCESS9_CODE_ADDR) - 0x08028000)) $< $@

$(BUILD)/$(TARGET)_unadjust.elf: $(PATCHES)
	$(LD) -i -T$(LDSCRIPT) $^ -o $@

$(BUILD)/payload/payload.elf: $(PAYLOAD_OBJS)
	$(LD) --use-blx -T$(LDSCRIPT) $(PAYLOAD_OBJS) -o $@

$(BUILD)/%.elf: $(BUILD)/%.o $(BUILD)/payload/payload.elf
	$(LD) -s -R $(BUILD)/payload/payload.elf -T$(LDSCRIPT) $< -o $@

$(BUILD)/payload/arm9/%.o: $(SOURCE)/payload/arm9/%.c
	$(CC) -mcpu=arm946e-s $(CFLAGS) -I$(SOURCE)/payload/include $< -c -o $@

$(BUILD)/payload/arm11/%.o: $(SOURCE)/payload/arm11/%.S
	$(CC) -mcpu=mpcore $(CFLAGS) -I$(SOURCE)/payload/include $< -c -o $@

$(BUILD)/%.o: $(SOURCE)/%.S
	$(CC) -mcpu=arm946e-s $(CFLAGS) $< -c -o $@

$(BUILD)/%.o: $(SOURCE)/%.s
	$(AS) $< -o $@
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD)
