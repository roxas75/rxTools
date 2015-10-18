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

TARGET	:= twl_firm
SOURCE	:= source
LD	:= arm-none-eabi-ld
OBJCOPY	:= arm-none-eabi-objcopy

.PHONY: clean all

all: $(BUILD)/$(TARGET).elf

$(BUILD):
	@[ -d $@ ] || mkdir -p $@

%.elf: %_unadjust.elf
	$(OBJCOPY) -S --change-section-address .patch.p9.*+$$((0x08014A00 - 0x08020000)) $< $@

$(BUILD)/$(TARGET)_unadjust.elf: $(BUILD)/p9_signatures_spoof.o
	$(LD) -i -Tscript.ld $^ -o $@

$(BUILD)/%.o: $(SOURCE)/%.s $(BUILD)
	$(AS) $< -o $@
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD)
