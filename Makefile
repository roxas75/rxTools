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

CFLAGS = -std=c11 -O2 -Wall -Wextra
ROPFLAGS = $(SET_DATNAME) DISPNAME=rxTools GRAPHICS=../logo
BRAHFLAGS = name=$(CODE_FILE) filepath=$(SYS_PATH)/ \
			APP_TITLE="rxTools" \
			APP_DESCRIPTION="Roxas75 3DS Toolkit & Custom Firmware" \
			APP_AUTHOR="Patois, et al." \
			ICON=$(abspath icon.png)

.PHONY: rxtools/build/$(CODE_FILE) clean distclean release all-target-patches	\
	all-target-mset	all-target-brahma all-target-theme release-licenses	\
	release-code release-doc release-patches release-themes-langs	\
	release-tools release-mset release-brahma

rxtools/build/$(CODE_FILE):
	@$(MAKE) $(SET_SYS_PATH) -C rxtools $(susbst rxtools/,,$@)

distclean:
	@rm -rf release

clean: distclean
	@$(MAKE) $(SET_SYS_PATH) -C rxtools clean
	@$(MAKE) -C rxmode clean
	@$(MAKE) -C reboot clean
	@$(MAKE) $(BRAHFLAGS) -C CakeBrah clean
	@$(MAKE) -C theme clean
	@$(MAKE) $(ROPFLAGS) -C CakesROP clean
	@$(MAKE) $(SET_DATNAME) -C CakesROP/CakesROPSpider clean	

release: all-target-patches all-target-mset all-target-brahma all-target-theme \
	release-licenses release-code release-doc release-patches release-themes-langs \
	release-tools release-mset release-brahma

release-licenses:
	@mkdir -p release
	@cp LICENSE release
	@cp LICENSE_JSMN release
	@cp CakeHax/LICENSE.txt release/LICENSE_CakeHax.txt
	@cp CakesROP/LICENSE release/LICENSE_CakesROP

release-code: rxtools/build/$(CODE_FILE)
	@mkdir -p release/$(SYS_PATH)
	@cp $< release/$(SYS_PATH)

release-doc:
	@cp doc/QuickStartGuide.pdf doc/rxTools.pdf release/

release-patches: reboot/reboot.bin all-target-patches
	@mkdir -p release/$(SYS_PATH) release/$(PATCHES_PATH)
	@cp reboot/reboot.bin release/$(SYS_PATH)
	@cp rxmode/build/ctr/native_firm.elf release/$(PATCHES_PATH)/0004013800000002.elf
	@cp rxmode/build/ctr/twl_firm.elf release/$(PATCHES_PATH)/0004013800000102.elf
	@cp rxmode/build/ctr/agb_firm.elf release/$(PATCHES_PATH)/0004013800000202.elf
	@cp rxmode/build/ktr/native_firm.elf release/$(PATCHES_PATH)/0004013820000002.elf

release-themes-langs:
	@mkdir -p release/rxTools/theme/0 release/rxTools/lang release/$(SYS_PATH)
	@mv theme/*.bin release/rxTools/theme/0
	@cp theme/LANG.txt tools/themetool.sh tools/themetool.bat release/rxTools/theme/0
	@cp rxtools/build/font.bin release/$(SYS_PATH)
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

all-target-mset:
	@$(MAKE) $(ROPFLAGS) -C CakesROP
	@$(MAKE) $(SET_DATNAME) -C CakesROP/CakesROPSpider

all-target-brahma:
	$(MAKE) $(BRAHFLAGS) -C CakeBrah

reboot/reboot.bin:
	$(MAKE) -C $(dir $@)

rxmode/build/%:
	$(MAKE) -C rxmode $(subst rxmode/,,$@)

.PHONY: all-target-theme
all-target-theme:
	@$(MAKE) -C theme
