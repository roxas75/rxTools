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

TARGET	:= twl_firm
SOURCE	:= source
LD	:= arm-none-eabi-ld
OBJCOPY	:= arm-none-eabi-objcopy

.PHONY: clean all

all: $(BUILD)/$(TARGET).elf

$(BUILD):
	$(call Q,MKDIR,$@)mkdir -p $@

%.elf: %_unstrip.elf
	$(call Q,OBJCOPY,$@)$(OBJCOPY) -S -R .text -R .data -R .bss -R .ARM.attributes $< $@

%_unstrip.elf: %_linked.elf
	$(call Q,LINK,$@)$(LD) -i -Tscript.ld $< -o $@

$(BUILD)/$(TARGET)_linked.elf: $(BUILD)/p9_signatures_spoof.o $(BUILD)/p9_sha_compare_stub.o $(BUILD)/p9_whitelist_check_disable.o $(BUILD)/p9_cartridge_save_type_check_disable.o $(BUILD)/p9_dsi_cartridge_save_exploit_check_disable.o $(BUILD)/p9_ninlogo_check_disable.o $(BUILD)/p9_blacklist_check_disable.o
	$(call Q,LINK,$@)$(LD) -Tscript.ld $^ -o $@

$(BUILD)/%.o: $(SOURCE)/%.s $(BUILD)
	$(call Q,AS,$@)$(AS) $< -o $@
#---------------------------------------------------------------------------------
clean:
	$(call Q,RM,)rm -fr $(BUILD)
