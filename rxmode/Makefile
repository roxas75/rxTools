#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
TARGET 		:=  patch.bin
BUILD		:=	build
ARMIPS    	:=  tools/armips.exe
SFILES 		 =  $(wildcard source/*.s)

all: 
	
#all the targets here
	@$(foreach sfile,$(SFILES), $(ARMIPS) $(sfile);)
	@tools\cfwtool.exe $(BUILD) $(TARGET)
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD)
 
