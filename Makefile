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

export GIT_VERSION := $(shell git rev-parse --short HEAD)

ifeq ($(GIT_VERSION),)
    GIT_VERSION := "N/A"
endif

CFLAGS = -std=c11 -O2 -Wall -Wextra
ROPFLAGS = $(SET_DATNAME) DISPNAME=rxTools GRAPHICS=../logo
BRAHFLAGS = name=$(CODE_FILE) filepath=$(SYS_PATH)/ \
			APP_TITLE='rxTools' \
			APP_DESCRIPTION='Roxas75 3DS Toolkit & Custom Firmware' \
			APP_AUTHOR='Patois, et al.' \
			ICON=$(abspath icon.png)

.PHONY: all-target-patches all-target-theme all-target-mset all-target-brahma	\
	reboot/reboot.bin clean distclean release	\
	release-licenses release-doc release-lang release-patches	\
	release-theme release-tools release-mset release-brahma

all-target-brahma:
	$(MAKE) $(BRAHFLAGS) -C CakeBrah

all-target-mset:
	@$(MAKE) $(ROPFLAGS) -C CakesROP
	@$(MAKE) $(SET_DATNAME) -C CakesROP/CakesROPSpider

all-target-patches: rxmode
	@$(MAKE) -C $<

all-target-rxtools: rxtools
	@$(MAKE) $(SET_SYS_PATH) -C $<

all-target-theme:
	@$(MAKE) -C theme

reboot/reboot.bin:
	$(MAKE) -C $(dir $@)

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

release: release-licenses release-rxtools release-doc release-lang	\
	release-patches release-theme release-tools release-mset release-brahma

release-licenses:
	@mkdir -p release
	@cp LICENSE release
	@cp rxtools/source/lib/jsmn/LICENSE release/LICENSE_JSMN
	@cp rxtools/CakeHax/LICENSE.txt release/LICENSE_CakeHax.txt
	@cp CakesROP/LICENSE release/LICENSE_CakesROP

release-rxtools: all-target-rxtools
	@mkdir -p release/$(SYS_PATH)
	@cp rxtools/build/code.bin release/$(SYS_PATH)
	@cp rxtools/build/font.bin release/$(SYS_PATH)

release-doc:
	@cp README.md "docs/QuickStartGuide(v3.0_BETA).pdf" release

release-lang:
	mkdir -p release/rxTools/lang
	@cp lang/* release/rxTools/lang

release-patches: reboot/reboot.bin all-target-patches
	@mkdir -p release/$(SYS_PATH) release/$(PATCHES_PATH)
	@cp reboot/reboot.bin release/$(SYS_PATH)
	@cp rxmode/build/ctr/native_firm.elf release/$(PATCHES_PATH)/0004013800000002.elf
	@cp rxmode/build/ctr/twl_firm.elf release/$(PATCHES_PATH)/0004013800000102.elf
	@cp rxmode/build/ctr/agb_firm.elf release/$(PATCHES_PATH)/0004013800000202.elf
	@cp rxmode/build/ktr/native_firm.elf release/$(PATCHES_PATH)/0004013820000002.elf

release-theme: all-target-theme
	mkdir -p release/rxTools/theme/0
	@mv theme/*.bin release/rxTools/theme/0
	@cp theme/LANG.txt scripts/themetool.sh scripts/themetool.bat release/rxTools/theme/0

release-tools:
	@mkdir -p release/scripts
	@cp scripts/o3ds_cdn_firm.py scripts/n3ds_cdn_firm.py scripts/readme.txt release/scripts

release-mset: all-target-mset
	@mkdir -p release/mset
	@cp CakesROP/CakesROP.nds release/mset/rxinstaller.nds
	@cp CakesROP/CakesROPSpider/code.bin release/mset/rxinstaller.bin

release-brahma: all-target-brahma
	@mkdir -p release/ninjhax/rxTools
	@cp CakeBrah/code.bin.3dsx release/ninjhax/rxTools/rxTools.3dsx
	@cp CakeBrah/code.bin.smdh release/ninjhax/rxTools/rxTools.smdh
