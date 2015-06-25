CC=arm-none-eabi-gcc
CP=Carm-none-eabi-g++
OC=arm-none-eabi-objcopy
LD=arm-none-eabi-ld
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

TARGET 		:=  mset.bin
BUILD		:=	build
ARMIPS    	:=  armips

IV     = 0160A0E14ADF4DE2EC109FE50250A0E1
KEY    = 9113BC189113AC189113AC3C00002400
IV2    = 00000000000000000000000000000000
KEY2   = 0160A0E14ADF4DE2EC109FE50250A0E1 

.PHONY: $(BUILD) clean all

all: rxinstaller.nds arm9_code.bin mset.bin

rxinstaller.nds:
	@cd rxinstaller && make

arm9_code.bin:
	@mkdir -p $(BUILD)
	@cd payload && make
	@mv payload.bin build/arm9_code.bin

mset.bin:
	armips mset4/arm9hax.s
	armips mset4/arm11hax.s
	armips mset4/rop.s
	openssl enc -aes-128-cbc -K $(KEY2) -iv $(IV2) -in build/rop.bin -out $(TARGET)

clean:
	@echo clean ...
	@cd rxinstaller && make clean
	@cd payload && make clean
	@rm -fr $(BUILD) *.nds *.bin
