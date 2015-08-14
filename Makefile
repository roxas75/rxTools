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

CODE_FILE := code.bin
CODE_PATH := rxTools/system/
SET_CODE_PATH := CODE_PATH=$(CODE_PATH)$(CODE_FILE)

INCDIR := -I$(CURDIR)/include
SET_INCDIR := INCDIR=$(INCDIR)

CFLAGS = -std=c11 -O2 -Wall -Wextra
CAKEFLAGS = dir_out=$(CURDIR) name=$(CODE_FILE) filepath=$(CODE_PATH)
#CAKEFLAGS = dir_out=$(CURDIR) name=$(CODE_FILE)

all: $(CODE_FILE)

.PHONY: distclean
distclean:
	@rm -rf release

.PHONY: clean
clean: distclean
	@$(MAKE) -C rxtools clean
	@$(MAKE) -C rxmode clean
	@$(MAKE) -C reboot clean
	@$(MAKE) $(SET_CODE_PATH) -C brahma clean
	@$(MAKE) -C theme clean
	@$(MAKE) $(SET_CODE_PATH) -C rxinstaller clean
	@$(MAKE) $(CAKEFLAGS) -C CakeHax clean
	@rm -Rf payload.bin $(CODE_FILE)

release: $(CODE_FILE) rxtools/font.bin reboot/reboot.bin \
	$(addprefix rxmode/build/,ktr/native_firm.elf	\
		ctr/native_firm.elf ctr/agb_firm.elf ctr/twl_firm.elf)	\
	all-target-brahma all-target-theme rxinstaller.nds
	@mkdir -p release
	@cp LICENSE release
	@cp LICENSE_JSMN release
	@cp CakeHax/LICENSE.txt release/LICENSE_CakeHax.txt
	@cp rxinstaller/LICENSE release/LICENSE_CakesROP

	@mkdir -p release/mset release/ninjhax release/rxTools
	@cp brahma/brahma.3dsx release/ninjhax/rxtools.3dsx
	@cp brahma/brahma.smdh release/ninjhax/rxtools.smdh
	@cp rxinstaller.nds release/mset/rxinstaller.nds
	@cp rxinstaller/CakesROPSpider/code.bin release/mset/code.bin

	@mkdir -p release/rxTools/system release/rxTools/theme

	@cp $(CODE_FILE) release/$(CODE_PATH)
	@cp rxtools/font.bin release/rxTools/system
	@cp reboot/reboot.bin release/rxTools/system

	@mkdir -p release/rxTools/system/patches/ctr release/rxTools/system/patches/ktr
	@cp rxmode/build/ctr/native_firm.elf release/rxTools/system/patches/ctr
	@cp rxmode/build/ctr/agb_firm.elf release/rxTools/system/patches/ctr
	@cp rxmode/build/ctr/twl_firm.elf release/rxTools/system/patches/ctr

	@cp rxmode/build/ktr/native_firm.elf release/rxTools/system/patches/ktr

	@mkdir -p release/rxTools/theme/0 release/rxTools/lang release/Tools/fbi_injection release/Tools/scripts
	@mv theme/*.bin release/rxTools/theme/0
	@cp theme/LANG.txt tools/themetool.sh tools/themetool.bat release/rxTools/theme/0
	@cp lang/* release/rxTools/lang/
	@cp tools/cdn_firm.py tools/msetdg.py tools/readme.txt release/Tools
	@cp -r tools/fbi_injection/* release/Tools/fbi_injection/
	@cp tools/scripts/* release/Tools/scripts/

	@cp doc/QuickStartGuide.pdf doc/rxTools.pdf release/

$(CODE_FILE): rxtools/rxtools.bin
	@$(MAKE) $(CAKEFLAGS) -C CakeHax bigpayload
	@dd if=rxtools/rxtools.bin of=$@ seek=272 conv=notrunc

rxinstaller.nds:
	@$(MAKE) $(SET_CODE_PATH) $(SET_INCDIR) -C rxinstaller

all-target-brahma:
	$(MAKE) $(SET_CODE_PATH) -C brahma

reboot/reboot.bin:
	$(MAKE) -C $(dir $@)

rxmode/build/%:
	$(MAKE) $(SET_INCDIR) -C rxmode $(subst rxmode/,,$@)

rxtools/rxtools.bin:
	@$(MAKE) -C $(dir $@) all
	@dd if=$@ of=$@ bs=896K count=1 conv=sync,notrunc

.PHONY: all-target-theme
all-target-theme:
	@$(MAKE) -C theme

.PHONY: rxtools/font.bin
rxtools/font.bin:
	@$(MAKE) -C $(dir $@) $(notdir $@)
