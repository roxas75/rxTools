#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

.PHONY: $(BUILD) clean all

all: nat_patch.bin agb_patch.bin twl_patch.bin

nat_patch.bin:
	@cd native_firm && $(MAKE)
	@cp native_firm/patch.bin $@
	
agb_patch.bin:
	@cd agb_firm && $(MAKE)
	@cp agb_firm/patch.bin $@

twl_patch.bin:
	@cd twl_firm && $(MAKE)
	@cp twl_firm/patch.bin $@
	
clean:
	@echo clean ...
	@cd native_firm && $(MAKE) clean
	@cd agb_firm && $(MAKE) clean
	@cd twl_firm && $(MAKE) clean
	@rm -fr $(BUILD) *.bin
