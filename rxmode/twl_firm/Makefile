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

%.elf: %_unstrip.elf
	$(OBJCOPY) -S -R .text -R .data -R .bss -R .ARM.attributes $< $@

%_unstrip.elf: %_linked.elf
	$(LD) -i -Tscript.ld $< -o $@

$(BUILD)/$(TARGET)_linked.elf: $(BUILD)/p9_signatures_spoof.o
	$(LD) -Tscript.ld $^ -o $@

$(BUILD)/%.o: $(SOURCE)/%.s $(BUILD)
	$(AS) $< -o $@
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD)
