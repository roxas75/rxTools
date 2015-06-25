PYTHON = python

CFLAGS = -std=c11 -O2 -Wall -Wextra

tools = tools/fill_with_zero tools/addxor_tool tools/cfwtool tools/pack_tool tools/xor tools/fill_with_crap

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
	@make -C msethax clean
	@rm -f $(tools) payload.bin data.bin rxTools.dat

.PHONY: release
release: rxTools.dat brahma/brahma.3dsx brahma/brahma.smdh
	@mkdir -p release/mset release/ninjhax
	@cp rxTools.dat release
	@cp brahma/brahma.3dsx release/ninjhax/rxtools.3dsx
	@cp brahma/brahma.smdh release/ninjhax/rxtools.smdh
	@cp msethax/rxinstaller.nds release/mset/rxinstaller.nds

rxTools.dat: payload.bin data.bin msethax/mset.bin tools/fill_with_crap
	@cp spiderhax/Launcher.dat $@
	@$(PYTHON) tools/insert.py $@ payload.bin 0x20000
	@$(PYTHON) tools/insert.py $@ data.bin 0x100000
	@$(PYTHON) tools/insert.py $@ msethax/mset.bin 0
	@tools/fill_with_crap $@ 4194304

.PHONY: brahma/brahma.3dsx brahma/brahma.smdh
brahma/brahma.3dsx brahma/brahma.smdh:
	make -C $(dir $@) all

.PHONY: msethax/mset.bin
msethax/mset.bin:
	make -C $(dir $@) all

data.bin: data/titlekey.bin data/reboot.bin rxmode/patch.bin data/top_bg.bin tools/pack_tool tools/xor
	@tools/pack_tool $(filter %.bin, $^) $@
	@tools/xor $@ tools/xorpad/data.xor
	@rm $@
	@mv $@.out $@

.PHONY: rxmode/patch.bin
rxmode/patch.bin: tools/cfwtool
	@make -C $(dir $@) all

payload.bin: rxtools/rxtools.bin tools/addxor_tool
	@tools/addxor_tool $< $@ 0x67893421 0x12756342

.PHONY: rxtools/rxtools.bin
rxtools/rxtools.bin: tools/fill_with_zero tools/addxor_tool
	@make -C $(dir $@) all
	@tools/fill_with_zero $@ 917504

$(tools): tools/%: tools/toolsrc/%/main.c
	$(LINK.c) $(OUTPUT_OPTION) $^
