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

ifeq ($(strip $(RXTOOLSMK)),)
$(error "Please run `make` at the top directory")
endif

include $(RXTOOLSMK)

include $(DEVKITARM)/base_rules

TARGET	:= native_firm
SOURCE	:= source
LD	:= arm-none-eabi-ld
OBJCOPY	:= arm-none-eabi-objcopy

CFLAGS	:= -Os -fshort-wchar -fno-zero-initialized-in-bss $(INCDIR) -I$(SOURCE)/include -DPATCHES_PATH=\"$(PATCHES_PATH)\"

OBJS	:= $(BUILD)/payload/arm9/EmuNAND.o $(BUILD)/payload/arm9/process9.o	\
	$(BUILD)/payload/arm9/create_thread.o	\
	$(BUILD)/payload/arm9/thread/thread.o $(BUILD)/k9_fix_flags.o	\
	$(BUILD)/p9_emunand_hook_1.o $(BUILD)/p9_emunand_hook_2.o	\
	$(BUILD)/p9_thread_hook.o	\
	$(BUILD)/p9_reboothax_entry.o $(BUILD)/p9_reboothax_body_prefix.o	\
	$(BUILD)/p9_signatures_spoof_1.o $(BUILD)/p9_signatures_spoof_2.o	\
	$(BUILD)/k11_codeset.o

ifdef PLATFORM_KTR
LDSCRIPT_ROM	:= ktr_rom.ld
LDSCRIPT_RAM	:= ktr_ram.ld
CFLAGS	+= -DPLATFORM_KTR

OBJS	+= $(BUILD)/k11_entry.o
else
LDSCRIPT_ROM	:= ctr_rom.ld
LDSCRIPT_RAM	:= ctr_ram.ld

OBJS	+= $(BUILD)/payload/arm9/thread/handlers.o	\
	$(BUILD)/payload/arm11/hookswi.o	\
	$(BUILD)/p9_keyx_spoof.o
endif

$(call DEPDIR,$(BUILD)/$(TARGET).elf $(OBJS))

.PHONY: clean all

all: $(BUILD)/$(TARGET).elf

%.elf: %_unstrip.elf
	$(call Q,OBJCOPY,$@)$(OBJCOPY) -S -R .ARM.attributes -R .comment $< $@

%_unstrip.elf: %_ram.elf
	$(call Q,LINK,$@)$(LD) -i -T$(LDSCRIPT_ROM) $< -o $@

$(BUILD)/$(TARGET)_ram.elf: $(OBJS)
	$(call Q,LINK,$@)$(LD) --use-blx -T$(LDSCRIPT_RAM) $^ -o $@

$(BUILD)/p9_reboothax_body_prefix.o: $(BUILD)/p9_reboothax_body.o
	$(call Q,OBJCOPY,$@)$(OBJCOPY) --prefix-alloc-sections .patch.p9.reboot.body $< $@

$(BUILD)/payload/arm11/%.o: $(SOURCE)/payload/arm11/%.S
	$(call Q,CPPAS,$@)$(CC) -mcpu=mpcore $(CFLAGS) $< -c -o $@

$(BUILD)/%.o: $(SOURCE)/%.c
	$(call Q,CC,$@)$(CC) -mcpu=arm946e-s $(CFLAGS) $< -c -o $@

$(BUILD)/%.o: $(SOURCE)/%.S
	$(call Q,CPPAS,$@)$(CC) -mcpu=arm946e-s $(CFLAGS) $< -c -o $@

$(BUILD)/%.o: $(SOURCE)/%.s
	$(call Q,AS,$@)$(AS) $< -o $@
#---------------------------------------------------------------------------------
clean:
	$(call Q,RM,)rm -fr $(BUILD)
