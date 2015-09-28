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
SYSTEM_PATH := rxTools/sys
SET_SYSTEM_PATH := SYSTEM_PATH=$(SYSTEM_PATH)
SET_CODE_PATH := CODE_PATH=$(SYSTEM_PATH)/$(CODE_FILE)
SET_DATNAME := DATNAME=$(SYSTEM_PATH)/$(CODE_FILE)

INCDIR := -I$(CURDIR)/include
SET_INCDIR := INCDIR=$(INCDIR)

CFLAGS = -std=c11 -O2 -Wall -Wextra
CAKEFLAGS = dir_out=$(CURDIR) name=$(CODE_FILE) filepath=$(SYSTEM_PATH)/
ROPFLAGS = $(SET_DATNAME) DISPNAME=rxTools GRAPHICS=../logo
BRAHFLAGS = name=$(CODE_FILE) filepath=$(SYSTEM_PATH)/ \
			APP_TITLE="rxTools" \
			APP_DESCRIPTION="Roxas75 3DS Toolkit & Custom Firmware" \
			APP_AUTHOR="Patois, et al." \
			ICON=$(abspath icon.png)

all: $(CODE_FILE)

.PHONY: distclean
distclean:
	@rm -rf release

.PHONY: clean
clean: distclean
	@$(MAKE) $(SET_SYSTEM_PATH) -C rxtools clean
	@$(MAKE) -C rxmode clean
	@$(MAKE) -C reboot clean
	@$(MAKE) $(BRAHFLAGS) -C CakeBrah clean
	@$(MAKE) -C theme clean
	@$(MAKE) $(ROPFLAGS) -C CakesROP clean
	@$(MAKE) $(SET_DATNAME) -C CakesROP/CakesROPSpider clean	
	@$(MAKE) $(CAKEFLAGS) -C CakeHax clean
	@rm -Rf payload.bin $(CODE_FILE)

release: $(CODE_FILE) all-target-patches all-target-mset all-target-brahma all-target-theme \
	release-licenses release-code release-doc release-patches release-themes-langs \
	release-tools release-mset release-brahma

release-licenses:
	@mkdir -p release
	@cp LICENSE release
	@cp LICENSE_JSMN release
	@cp CakeHax/LICENSE.txt release/LICENSE_CakeHax.txt
	@cp CakesROP/LICENSE release/LICENSE_CakesROP

release-code:
	@mkdir -p release/$(SYSTEM_PATH)
	@cp $(CODE_FILE) release/$(SYSTEM_PATH)

release-doc:
	@cp doc/QuickStartGuide.pdf doc/rxTools.pdf release/

release-patches:
	@mkdir -p release/$(SYSTEM_PATH)/patches/ctr release/$(SYSTEM_PATH)/patches/ktr
	@cp reboot/reboot.bin release/$(SYSTEM_PATH)
	@cp rxmode/build/ctr/native_firm.elf release/$(SYSTEM_PATH)/patches/ctr
	@cp rxmode/build/ctr/agb_firm.elf release/$(SYSTEM_PATH)/patches/ctr
	@cp rxmode/build/ctr/twl_firm.elf release/$(SYSTEM_PATH)/patches/ctr
	@cp rxmode/build/ktr/native_firm.elf release/$(SYSTEM_PATH)/patches/ktr

release-themes-langs:
	@mkdir -p release/rxTools/theme/0 release/rxTools/lang release/$(SYSTEM_PATH)
	@mv theme/*.bin release/rxTools/theme/0
	@cp theme/LANG.txt tools/themetool.sh tools/themetool.bat release/rxTools/theme/0
	@cp rxtools/font.bin release/$(SYSTEM_PATH)
	@cp lang/* release/rxTools/lang/

release-tools:
	@mkdir -p release/Tools/fbi_injection release/Tools/scripts
	@cp tools/cdn_firm.py tools/readme.txt release/Tools
	@cp -r tools/fbi_injection/* release/Tools/fbi_injection/
	@cp tools/scripts/* release/Tools/scripts/

release-mset:
	@mkdir -p release/mset
	@cp CakesROP/CakesROP.nds release/mset/rxinstaller.nds
	@cp CakesROP/CakesROPSpider/code.bin release/mset/rxinstaller.bin

release-brahma:
	@mkdir -p release/ninjhax/rxTools
	@cp CakeBrah/code.bin.3dsx release/ninjhax/rxTools/rxtools.3dsx
	@cp CakeBrah/code.bin.smdh release/ninjhax/rxTools/rxtools.smdh

all-target-patches:	reboot/reboot.bin \
	$(addprefix rxmode/build/,ktr/native_firm.elf \
		ctr/native_firm.elf ctr/agb_firm.elf ctr/twl_firm.elf)

$(CODE_FILE): rxtools/rxtools.bin
	@$(MAKE) $(CAKEFLAGS) -C CakeHax bigpayload
	@dd if=rxtools/rxtools.bin of=$@ seek=160 conv=notrunc

all-target-mset:
	@$(MAKE) $(ROPFLAGS) -C CakesROP
	@$(MAKE) $(SET_DATNAME) -C CakesROP/CakesROPSpider

all-target-brahma:
	$(MAKE) $(BRAHFLAGS) -C CakeBrah

reboot/reboot.bin:
	$(MAKE) -C $(dir $@)

rxmode/build/%:
	$(MAKE) $(SET_INCDIR) -C rxmode $(subst rxmode/,,$@)

rxtools/rxtools.bin:
	@$(MAKE) $(SET_SYSTEM_PATH) -C $(dir $@) all
	@dd if=$@ of=$@ bs=896K count=1 conv=sync,notrunc

.PHONY: all-target-theme
all-target-theme:
	@$(MAKE) -C theme
