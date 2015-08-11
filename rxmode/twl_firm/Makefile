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

TARGET 		:=  twl_firm
SOURCE		:=	source
LD		:=	arm-none-eabi-ld

.PHONY: clean all

all: $(BUILD)/$(TARGET).elf

$(BUILD):
	@[ -d $@ ] || mkdir -p $@

%.elf: %_unstrip.elf
	$(STRIP) $< -o $@

$(BUILD)/$(TARGET)_unstrip.elf: $(BUILD)/p9_signatures_spoof.o
	$(LD) -i -Tscript.ld $^ -o $@

$(BUILD)/%.o: $(SOURCE)/%.s $(BUILD)
	$(AS) $< -o $@
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD)
