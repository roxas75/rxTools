# Copyright (C) 2015 The PASTA Team
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

CFLAGS = -std=c11 -O2 -Wall -Wextra
CAKEFLAGS = dir_out=$(CURDIR) name=rxTools.dat

tools = tools/addxor_tool tools/cfwtool tools/pack_tool tools/xor tools/font_tool
RXMODE_TARGETS = rxmode/nat_patch.bin rxmode/agb_patch.bin rxmode/twl_patch.bin
DATA_FILES := $(wildcard data/*.*) data/reboot/reboot.bin $(RXMODE_TARGETS)

all: rxTools.dat

.PHONY: distclean
distclean: clean
	@rm -rf release/rxTools.dat release/ninjhax release/mset

.PHONY: clean
clean:
	@$(MAKE) -C rxtools clean
	@$(MAKE) -C rxmode clean
	@$(MAKE) -C brahma clean
	@$(MAKE) -C theme clean
	@$(MAKE) -C rxinstaller clean
	@$(MAKE) $(CAKEFLAGS) -C CakeHax clean
	@rm -f $(tools) payload.bin data.bin rxTools.dat

release: rxTools.dat rxtools/font.bin all-target-brahma all-target-theme rxinstaller.nds
	@mkdir -p release/mset release/ninjhax release/rxTools
	@cp rxTools.dat release
	@cp brahma/brahma.3dsx release/ninjhax/rxtools.3dsx
	@cp brahma/brahma.smdh release/ninjhax/rxtools.smdh
	@cp rxinstaller.nds release/mset/rxinstaller.nds

	@mkdir -p release/rxTools/theme/0
	@mv theme/*.bin release/rxTools/theme/0
	@cp theme/LANG.txt tools/themetool.sh tools/themetool.bat release/rxTools/theme/0
	@cp rxtools/font.bin release/rxTools

	@cp doc/QuickStartGuide.pdf doc/rxTools.pdf release/

rxTools.dat: rxtools/rxtools.bin data.bin
	@$(MAKE) $(CAKEFLAGS) -C CakeHax bigpayload
	@dd if=rxtools/rxtools.bin of=$@ seek=272 conv=notrunc
	@dd if=data.bin of=$@ seek=2K conv=notrunc

rxinstaller.nds:
	@$(MAKE) -C rxinstaller

all-target-brahma:
	$(MAKE) -C brahma

data.bin: tools/pack_tool $(DATA_FILES)
	@tools/pack_tool $(DATA_FILES) $@

data/reboot/reboot.bin:
	$(MAKE) -C $(dir $@) $(notdir $@)

$(RXMODE_TARGETS): tools/cfwtool
	$(MAKE) -C $(dir $@) $(notdir $@)

rxtools/rxtools.bin: tools/addxor_tool tools/font_tool
	@$(MAKE) -C $(dir $@) all
	@dd if=$@ of=$@ bs=896K count=1 conv=sync,notrunc

.PHONY: all-target-theme
all-target-theme:
	@$(MAKE) -C theme

.PHONY: rxtools/font.bin
rxtools/font.bin: tools/font_tool
	@$(MAKE) -C $(dir $@) $(notdir $@)

$(tools): tools/%: tools/toolsrc/%/main.c
	$(LINK.c) $(OUTPUT_OPTION) $^
