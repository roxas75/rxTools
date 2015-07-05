PYTHON = python

CFLAGS = -std=c11 -O2 -Wall -Wextra
CAKEFLAGS = dir_out=$(CURDIR) name=rxTools.dat

tools = tools/addxor_tool tools/cfwtool tools/pack_tool tools/xor tools/font_tool
DATA_FILES := $(wildcard data/*.*) rxmode/nat_patch.bin rxmode/agb_patch.bin rxmode/twl_patch.bin

.PHONY: all
all: rxTools.dat

.PHONY: distclean
distclean: clean
	@rm -rf release/rxTools.dat release/ninjhax release/mset

.PHONY: clean
clean:
	@make -C rxtools clean
	@make -C rxmode clean
	@make -C brahma clean
	@make -C theme clean
	## @make -C rxinstaller clean  ## rxInstaller not compiling from here. Disabled for now
	@make $(CAKEFLAGS) -C CakeHax clean
	@rm -f $(tools) payload.bin data.bin rxTools.dat

.PHONY: release
release: rxTools.dat brahma/brahma.3dsx brahma/brahma.smdh theme
	@mkdir -p release/mset release/ninjhax
	@cp rxTools.dat release
	@cp brahma/brahma.3dsx release/ninjhax/rxtools.3dsx
	@cp brahma/brahma.smdh release/ninjhax/rxtools.smdh
	##@cp rxinstaller.nds release/mset/rxinstaller.nds ## rxInstaller not compiling from here. Disabled for now.

.PHONY: rxTools.dat
rxTools.dat: rxtools/rxtools.bin rxmode/*.bin data.bin
	@make $(CAKEFLAGS) -C CakeHax bigpayload
	@dd if=rxtools/rxtools.bin of=$@ seek=272 conv=notrunc
	@dd if=data.bin of=$@ seek=2K conv=notrunc

## rxInstaller doesn't want to compile from main makefile at the moment. Compile it seperately until someone else fixes this. :P
# .PHONY: rxinstaller.nds
# rxinstaller.nds:
	# @make -C rxinstaller/rxinstaller all

.PHONY: brahma/brahma.3dsx brahma/brahma.smdh
brahma/brahma.3dsx brahma/brahma.smdh:
	make -C $(dir $@) all

data.bin: tools/pack_tool
	@tools/pack_tool $(DATA_FILES) $@
	
.PHONY: rxmode/*.bin
rxmode/*.bin: tools/cfwtool
	@cd rxmode && make

.PHONY: rxtools/rxtools.bin 
rxtools/rxtools.bin: tools/addxor_tool tools/font_tool
	@make -C $(dir $@) all
	@dd if=$@ of=$@ bs=896K count=1 conv=sync,notrunc

.PHONY: theme
theme:
	@cd theme && make
	@mkdir -p release/theme
	@mv theme/*.bin release/theme

$(tools): tools/%: tools/toolsrc/%/main.c
	$(LINK.c) $(OUTPUT_OPTION) $^
