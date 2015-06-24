#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
TARGET 		:=  patch.bin
BUILD		:=	build
TOOLS		:=	../tools
ARMIPS    	:=  armips
SFILES 		:=  $(wildcard source/*.s)

.PHONY: clean all

all: $(BUILD) arm9payload.bin patch.bin

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	
arm9payload.bin:
	@cd source/arm9 && make
	
patch.bin:
	@$(foreach sfile,$(SFILES), $(ARMIPS) $(sfile);)
	@$(TOOLS)/cfwtool $(BUILD) $(TARGET)
	@mv patch.bin ../data/patch.bin
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET) *.bin
 
