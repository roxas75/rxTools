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

#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

.PHONY: $(BUILD) clean all

all: nat_patch.bin agb_patch.bin twl_patch.bin

nat_patch.bin:
	@cd native_firm && make
	@cp native_firm/patch.bin $@

agb_patch.bin:
	@cd agb_firm && make
	@cp agb_firm/patch.bin $@

twl_patch.bin:
	@cd twl_firm && make
	@cp twl_firm/patch.bin $@

clean:
	@echo clean ...
	@cd native_firm && make clean
	@cd agb_firm && make clean
	@cd twl_firm && make clean
	@rm -fr $(BUILD) *.bin
