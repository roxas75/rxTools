#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
TARGET 		:=  patch.bin
BUILD		:=	build
TOOLS		:=	../tools
AS		:=	arm-none-eabi-as
OS		:=	arm-none-eabi-objcopy
SFILES 		:=	$(patsubst %.s,%,$(patsubst source/%,%,$(wildcard source/*.s)))

.PHONY: clean all

all: $(BUILD) arm9payload.bin patch.bin

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	
arm9payload.bin:
	@cd source/arm9 && make
	
patch.bin:
	@mkdir -p build
	@$(foreach sfile,$(SFILES), $(AS) -c -o build/$(sfile).bin	source/$(sfile).s $(ASFLAGS);)
	@$(foreach sfile,$(SFILES), $(OS) -O binary build/$(sfile).bin;)

	@$(TOOLS)/cfwtool $(BUILD) $(TARGET)
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET) *.bin
