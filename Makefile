#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif
#---------------------------------------------------------------------------------
TOOLS := tools

#---------------------------------------------------------------------------------

.PHONY: all clean

#i know, these binary do not exist. The purpouse is to update the package everytime

all: rxtools.bin rxmode.bin data.bin spiderhax.bin brahma.bin msethax.bin release.bin

rxtools.bin:
	@cls
	@echo BUILDING RXTOOLS...
	@cd rxtools && make
	@$(TOOLS)/fill_with_zero.exe rxtools\rxtools.bin 917504
	@$(TOOLS)/addxor_tool.exe rxtools\rxtools.bin payload.bin 0x67893421 0x12756342

rxmode.bin:
	@cls
	@echo BUILDING RXMODE...
	@cd rxmode && make

data.bin:
	@cls
	@echo BUILDING DATA...
	@$(TOOLS)/pack_tool.exe data/titlekey.bin data/reboot.bin data/patch.bin data/top_bg.bin data.bin
	@$(TOOLS)/xor.exe data.bin $(TOOLS)\xorpad\data.xor
	@rm data.bin
	@mv data.bin.out data.bin

spiderhax.bin:
	@cls
	@echo CREATING RXTOOLS.DAT...
	@cp spiderhax/Launcher.dat rxTools.dat
	@python $(TOOLS)/insert.py rxTools.dat payload.bin 0x20000
	@python $(TOOLS)/insert.py rxTools.dat data.bin 0x100000
	@$(TOOLS)/fill_with_crap.exe rxTools.dat 4194304
	@rm payload.bin data.bin

brahma.bin:
	@cls
	@echo BUILDING BRAHMA FOR NINJHAX...
	@cd brahma && make

msethax.bin:
	@cls
	@echo BUILDING MSETHAX...
	@cd msethax && make
	@python $(TOOLS)/insert.py rxTools.dat msethax\mset.bin 0

release.bin:
	@cls 
	@echo MAKING THE RELEASE...
	@mkdir -p release/mset
	@mkdir -p release/ninjhax
	@mv rxTools.dat release/rxTools.dat
	@cp brahma\brahma.3dsx release\ninjhax\rxtools.3dsx
	@cp brahma\brahma.smdh release\ninjhax\rxtools.smdh
	@cp msethax\rxinstaller.nds release\mset\rxinstaller.nds
	@echo DONE!!
clean:
	@cd rxtools && make clean
	@cd rxmode && make clean
	@cd brahma && make clean
	@cd msethax && make clean