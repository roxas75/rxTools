#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

TARGET 		:=  patch.bin
BUILD		:=	build
TOOLS		:=	../../tools
ARMIPS    	:=  armips
SFILES 		:=  $(wildcard source/*.s)

.PHONY: clean all

all: $(BUILD) arm9payload.bin patch.bin

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	
arm9payload.bin:
	
patch.bin:
	@$(foreach sfile,$(SFILES), $(ARMIPS) $(sfile);)
	@$(TOOLS)/cfwtool $(BUILD) $(TARGET) a
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET) *.bin
 
