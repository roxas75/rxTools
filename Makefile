PYTHON = python

CFLAGS = -std=c11 -O2 -Wall -Wextra

tools = tools/addxor_tool tools/cfwtool tools/pack_tool tools/xor
DATA_FILES := $(wildcard data/*.*) rxmode/nat_patch.bin rxmode/agb_patch.bin rxmode/twl_patch.bin

.PHONY: all
all: rxTools.dat

.PHONY: distclean
distclean: clean
	@rm -rf release/rxTools.dat release/ninjhax release/mset

.PHONY: clean
clean:
	@$(MAKE) -C rxtools clean
	@$(MAKE) -C rxmode clean
	@$(MAKE) -C brahma clean
	@$(MAKE) -C msethax clean
	@rm -f $(tools) payload.bin data.bin rxTools.dat

.PHONY: release
release: rxTools.dat brahma/brahma.3dsx brahma/brahma.smdh
	@mkdir -p release/mset release/ninjhax
	@cp rxTools.dat release
	@cp brahma/brahma.3dsx release/ninjhax/rxtools.3dsx
	@cp brahma/brahma.smdh release/ninjhax/rxtools.smdh
	@cp msethax/rxinstaller.nds release/mset/rxinstaller.nds

rxTools.dat: payload.bin rxmode/*.bin data.bin msethax/mset.bin
	@dd if=spiderhax/Launcher.dat of=$@ bs=4M conv=sync
	@dd if=msethax/mset.bin of=$@ conv=notrunc
	@dd if=payload.bin of=$@ seek=256 conv=notrunc
	@dd if=data.bin of=$@ seek=2K conv=notrunc

.PHONY: brahma/brahma.3dsx brahma/brahma.smdh
brahma/brahma.3dsx brahma/brahma.smdh:
	$(MAKE) -C $(dir $@) all

.PHONY: msethax/mset.bin
msethax/mset.bin:
	$(MAKE) -C $(dir $@) all

data.bin: tools/pack_tool tools/xor
	@tools/pack_tool $(DATA_FILES) $@
	@tools/xor $@ tools/xorpad/data.xor
	@rm $@
	@mv $@.out $@

.PHONY: rxmode/*.bin
rxmode/*.bin: tools/cfwtool
	@cd rxmode && $(MAKE)

payload.bin: rxtools/rxtools.bin tools/addxor_tool
	@tools/addxor_tool $< $@ 0x67893421 0x12756342

.PHONY: rxtools/rxtools.bin
rxtools/rxtools.bin: tools/addxor_tool
	@$(MAKE) -C $(dir $@) all
	@dd if=$@ of=$@ bs=896K count=1 conv=sync,notrunc

$(tools): tools/%: tools/toolsrc/%/main.c
	$(LINK.c) $(OUTPUT_OPTION) $^
