#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/base_rules

TARGET 		:=  patch.bin
BUILD		:=	build
SOURCE		:=	source
TOOLS		:=	../../tools
LD		:=	arm-none-eabi-ld

.PHONY: clean all

all: $(BUILD) arm9payload.bin patch.bin

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	
arm9payload.bin:
	
patch.bin: $(BUILD)/080238c0.bin
	@$(TOOLS)/cfwtool $(BUILD) $(TARGET) t

$(BUILD)/%.bin: $(BUILD)/%.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/%.elf: $(BUILD)/%.o
	$(LD) -e 0 -Ttext $(<:$(BUILD)/%.o=0x%) $< -o $@

$(BUILD)/080238c0.o: $(SOURCE)/p9_signatures_spoof.s
	$(AS) $< -o $@
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET) *.bin
 
