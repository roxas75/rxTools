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
SYS_PATH := rxTools/sys
SET_SYS_PATH := SYS_PATH=$(SYS_PATH)
export PATCHES_PATH := $(SYS_PATH)/patches
SET_CODE_PATH := CODE_PATH=$(SYS_PATH)/$(CODE_FILE)
SET_DATNAME := DATNAME=$(SYS_PATH)/$(CODE_FILE)

export INCDIR := -I$(CURDIR)/include
export RXTOOLSMK := $(CURDIR)/common.mk

CFLAGS = -std=c11 -O2 -Wall -Wextra
ROPFLAGS = $(SET_DATNAME) DISPNAME=rxTools GRAPHICS=../logo
BRAHFLAGS = name=$(CODE_FILE) filepath=$(SYS_PATH)/ \
			APP_TITLE="rxTools" \
			APP_DESCRIPTION="Roxas75 3DS Toolkit & Custom Firmware" \
			APP_AUTHOR="Patois, et al." \
			ICON=$(abspath icon.png)

RELEASE := build/release

.PHONY: all-target-patches all-target-theme all-target-mset all-target-brahma	\
	reboot/reboot.bin clean distclean release	\
	release-licenses release-doc release-lang release-patches	\
	release-theme release-tools release-mset release-brahma

all-target-brahma:
	@$(MAKE) $(BRAHFLAGS) -C CakeBrah

all-target-mset:
	@$(MAKE) $(ROPFLAGS) -C CakesROP
	@$(MAKE) $(SET_DATNAME) -C CakesROP/CakesROPSpider

all-target-patches: rxmode
	@$(MAKE) BUILD=../build/rxmode -C $<

all-target-rxtools: rxtools
	@$(MAKE) $(SET_SYS_PATH) BUILD=../build/rxtools -C $<

all-target-theme:
	@$(MAKE) -C theme

reboot/reboot.bin:
	$(MAKE) -C $(dir $@)

distclean:
	@rm -rf $(RELEASE)

clean:
	@rm -rf build
	@$(MAKE) -C reboot clean
	@$(MAKE) $(BRAHFLAGS) -C CakeBrah clean
	@$(MAKE) -C theme clean
	@$(MAKE) $(ROPFLAGS) -C CakesROP clean
	@$(MAKE) $(SET_DATNAME) -C CakesROP/CakesROPSpider clean	

release: release-licenses release-rxtools release-doc release-lang	\
	release-patches release-theme release-tools release-mset release-brahma

release-licenses:
	@mkdir -p $(RELEASE)
	@cp LICENSE $(RELEASE)
	@cp LICENSE_JSMN $(RELEASE)
	@cp rxtools/CakeHax/LICENSE.txt $(RELEASE)/LICENSE_CakeHax.txt
	@cp CakesROP/LICENSE $(RELEASE)/LICENSE_CakesROP

release-rxtools: all-target-rxtools
	@mkdir -p $(RELEASE)/$(SYS_PATH)
	@cp build/rxtools/code.bin build/rxtools/font.bin $(RELEASE)/$(SYS_PATH)

release-doc:
	@cp doc/QuickStartGuide.pdf doc/rxTools.pdf $(RELEASE)

release-lang:
	mkdir -p $(RELEASE)/rxTools/lang
	@cp lang/* $(RELEASE)/rxTools/lang

release-patches: reboot/reboot.bin all-target-patches
	@mkdir -p $(RELEASE)/$(SYS_PATH) $(RELEASE)/$(PATCHES_PATH)
	@cp reboot/reboot.bin $(RELEASE)/$(SYS_PATH)
	@cp build/rxmode/ctr/native_firm.elf $(RELEASE)/$(PATCHES_PATH)/0004013800000002.elf
	@cp build/rxmode/ctr/twl_firm.elf $(RELEASE)/$(PATCHES_PATH)/0004013800000102.elf
	@cp build/rxmode/ctr/agb_firm.elf $(RELEASE)/$(PATCHES_PATH)/0004013800000202.elf
	@cp build/rxmode/ktr/native_firm.elf $(RELEASE)/$(PATCHES_PATH)/0004013820000002.elf

release-theme: all-target-theme
	mkdir -p $(RELEASE)/rxTools/theme/0
	@mv theme/*.bin $(RELEASE)/rxTools/theme/0
	@cp theme/LANG.txt tools/themetool.sh tools/themetool.bat $(RELEASE)/rxTools/theme/0

release-tools:
	@mkdir -p $(RELEASE)/Tools/fbi_injection $(RELEASE)/Tools/scripts
	@cp tools/cdn_firm.py tools/readme.txt $(RELEASE)/Tools
	@cp -r tools/fbi_injection/* $(RELEASE)/Tools/fbi_injection/
	@cp tools/scripts/* $(RELEASE)/Tools/scripts/

release-mset: all-target-mset
	@mkdir -p $(RELEASE)/mset
	@cp CakesROP/CakesROP.nds $(RELEASE)/mset/rxinstaller.nds
	@cp CakesROP/CakesROPSpider/code.bin $(RELEASE)/mset/rxinstaller.bin

release-brahma: all-target-brahma
	@mkdir -p $(RELEASE)/ninjhax/rxTools
	@cp CakeBrah/code.bin.3dsx $(RELEASE)/ninjhax/rxTools/rxtools.3dsx
	@cp CakeBrah/code.bin.smdh $(RELEASE)/ninjhax/rxTools/rxtools.smdh
