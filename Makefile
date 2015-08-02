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
	@make -C rxinstaller clean
	@make $(CAKEFLAGS) -C CakeHax clean
	@rm -f $(tools) payload.bin data.bin rxTools.dat

.PHONY: release
release: rxTools.dat rxtools/font.bin brahma/brahma.3dsx brahma/brahma.smdh theme doc rxinstaller.nds
	@mkdir -p release/mset release/ninjhax
	@cp rxTools.dat release
	@cp rxtools/font.bin release/rxTools
	@cp brahma/brahma.3dsx release/ninjhax/rxtools.3dsx
	@cp brahma/brahma.smdh release/ninjhax/rxtools.smdh
	@cp rxinstaller.nds release/mset/rxinstaller.nds

.PHONY: rxTools.dat
rxTools.dat: rxtools/rxtools.bin rxmode/*.bin data.bin
	@make $(CAKEFLAGS) -C CakeHax bigpayload
	@dd if=rxtools/rxtools.bin of=$@ seek=272 conv=notrunc
	@dd if=data.bin of=$@ seek=2K conv=notrunc

.PHONY: rxinstaller.nds
rxinstaller.nds:
	@make -C rxinstaller

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
	@mkdir -p release/rxTools/theme/0
	@mv theme/*.bin release/rxTools/theme/0
	@cp theme/LANG.txt tools/themetool.sh tools/themetool.bat release/rxTools/theme/0

.PHONY: rxtools/font.bin
rxtools/font.bin: tools/font_tool
	@make -C rxtools font.bin

.PHONY: doc
doc:
	@cp doc/QuickStartGuide.pdf doc/rxTools.pdf release/

$(tools): tools/%: tools/toolsrc/%/main.c
	$(LINK.c) $(OUTPUT_OPTION) $^
