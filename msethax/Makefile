CC=arm-none-eabi-gcc
CP=Carm-none-eabi-g++
OC=arm-none-eabi-objcopy
LD=arm-none-eabi-ld
AS=arm-none-eabi-as
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TARGET 		:=  mset.bin
BUILD		:=	build

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
	@$(AS) -c -o build/arm9hax.bin mset4/arm9hax.s
	@$(OC) -O binary build/arm9hax.bin
	@$(AS) -c -o build/arm11hax.bin mset4/arm11hax.s
	@$(OC) -O binary build/arm11hax.bin
	@$(AS) -c -o build/rop.bin mset4/rop.s
	@$(OC) -O binary build/rop.bin
	openssl enc -aes-128-cbc -K $(KEY2) -iv $(IV2) -in build/rop.bin -out $(TARGET)

clean:
	@echo clean ...
	@cd rxinstaller && make clean
	@cd payload && make clean
	@rm -fr $(BUILD) *.nds *.bin
