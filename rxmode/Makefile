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

.PHONY: all clean

all: $(addprefix build/,ktr/native_firm.elf	\
	ctr/native_firm.elf ctr/agb_firm.elf ctr/twl_firm.elf)

build/ktr/native_firm.elf:
	$(MAKE) INCDIR=$(INCDIR) BUILD=$(CURDIR)/build/ktr PLATFORM_KTR=1 -C native_firm $(CURDIR)/$@

build/ctr/native_firm.elf:
	$(MAKE) INCDIR=$(INCDIR) BUILD=$(CURDIR)/build/ctr -C native_firm $(CURDIR)/$@

build/ctr/agb_firm.elf:
	$(MAKE) INCDIR=$(INCDIR) BUILD=$(CURDIR)/build/ctr -C agb_firm $(CURDIR)/$@

build/ctr/twl_firm.elf:
	$(MAKE) INCDIR=$(INCDIR) BUILD=$(CURDIR)/build/ctr -C twl_firm $(CURDIR)/$@

clean:
	rm -Rf build
