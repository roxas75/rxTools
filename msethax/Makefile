CC=arm-none-eabi-gcc
CP=Carm-none-eabi-g++
OC=arm-none-eabi-objcopy
LD=arm-none-eabi-ld
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
TARGET 		:=  mset.bin
BUILD		:=	build
ARMIPS    	:=  armips

IV     = 0160A0E14ADF4DE2EC109FE50250A0E1
KEY    = 9113BC189113AC189113AC3C00002400
IV2    = 00000000000000000000000000000000
KEY2   = 0160A0E14ADF4DE2EC109FE50250A0E1 

.PHONY: mset

mset:
	armips mset4/arm9hax.s
	armips mset4/arm11hax.s
#openssl enc -aes-128-cbc -K $(KEY) -iv $(IV) -in build/arm11hax.bin -out build/arm11hax_enc.bin
	armips mset4/rop.s
	openssl enc -aes-128-cbc -K $(KEY2) -iv $(IV2) -in build/rop.bin -out $(TARGET)
#@rm -fr $(BUILD)
#armips mset6/arm9hax.s
#armips mset6/arm11hax.s
#armips mset6/rop.s
#python ..\tools\insert.py $(TARGET) build/rop.bin 0x1000
	

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	
