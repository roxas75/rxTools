#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

.PHONY: $(BUILD) clean all

all: nat_patch.bin agb_patch.bin twl_patch.bin

nat_patch.bin:
	@cd native_firm && make
	@cp native_firm/patch.bin $@
	
agb_patch.bin:
	@cd agb_firm && make
	@cp agb_firm/patch.bin $@

twl_patch.bin:
	@cd twl_firm && make
	@cp twl_firm/patch.bin $@
	
clean:
	@echo clean ...
	@cd native_firm && make clean
	@cd agb_firm && make clean
	@cd twl_firm && make clean
	@rm -fr $(BUILD) *.bin
