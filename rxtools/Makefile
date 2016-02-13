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

ifeq ($(strip $(RXTOOLSMK)),)
$(error "Please run `make` at the top directory")
endif

BUILD := build
DEPS_DIR := $(BUILD)/.deps
SOURCE := source

all: $(BUILD)/code.bin

include $(RXTOOLSMK)

LD := $(CC)

INCDIR += -Isource -Isource/features -Isource/lib
CPPDEFS	:= -DSYS_PATH=\"$(SYS_PATH)\" -DPATCHES_PATH=\"$(PATCHES_PATH)\"
CPPFLAGS := $(INCDIR) $(CPPDEFS)

CFORCE := -fomit-frame-pointer -ffast-math
CMACHINE := -mtune=arm946e-s
CWARNING := -Wall -Wno-multichar
CFLAGS := -g -O2 -std=c99 $(CWARNING) $(CFORCE) $(CMACHINE) $(CPPFLAGS)

# Use Thumb libraries because sbrk of ARM newlib in devkitPro doesn't work.
LDFLAGS := -nostartfiles -g $(CMACHINE) -mthumb -mthumb-interwork -Wl,-Tstub.ld

CCOBJS := $(addprefix $(BUILD)/,main.o features/CTRDecryptor.o	\
	features/NandDumper.o features/TitleKeyDecrypt.o	\
	features/configuration.o features/downgradeapp.o	\
	features/AdvancedFileManager.o features/firm.o	\
	features/nandtools.o features/padgen.o features/screenshot.o	\
	lib/fatfs/diskio.o lib/fatfs/ff.o lib/fatfs/option/unicode.o	\
	lib/tmio/tmio.o	\
	lib/polarssl/padlock.o lib/polarssl/sha2.o lib/polarssl/aes.o	\
	lib/console.o lib/crypto.o lib/draw.o lib/fs.o lib/hid.o lib/lang.o	\
	lib/log.o lib/i2c.o lib/jsmn/jsmn.o lib/menu.o lib/nand.o lib/ncch.o	\
	features/reboot.o)

OBJS	:= $(CCOBJS) $(addprefix $(BUILD)/, start.o font_ascii.o lib/delay.o)
DEPS	:= $(subst $(BUILD)/,$(DEPS_DIR)/,$(CCOBJS))

$(call DEPDIR,$(BUILD)/rxtools.elf $(BUILD)/font.bin $(OBJS) $(DEPS))

MAKEDEP	= @$(COMPILE.c) -MM $< > $(patsubst $(BUILD)/%.o,$(DEPS_DIR)/%.d,$@)

.PHONY: $(BUILD)/code.bin all clean

$(BUILD)/code.bin: $(BUILD)/rxtools.bin $(BUILD)/CakeHax/D
	$(call Q,MAKE,CakeHax)$(MAKE) dir_out=../$(dir $@) dir_build=../$(BUILD)/CakeHax name=$(notdir $@) filepath=$(SYS_PATH)/ -C CakeHax bigpayload
	$(call Q,DD,$@)dd if=$< bs=896K conv=sync,notrunc | dd of=$@ seek=160 conv=notrunc

%.bin: %.elf
	$(call Q,OBJCOPY,$@)$(OBJCOPY) -O binary $< $@

$(BUILD)/rxtools.elf: $(OBJS)
	$(call Q,LINK,$@)$(LINK.o) $^ $(OUTPUT_OPTION)

$(BUILD)/%.o: $(SOURCE)/%.c
	$(call Q,CC,$@)$(COMPILE.c) $< $(OUTPUT_OPTION)
	$(MAKEDEP)

$(BUILD)/%.o: $(SOURCE)/%.S
	$(call Q,CPPAS,$@)$(COMPILE.c) $< $(OUTPUT_OPTION)
	$(MAKEDEP)

$(BUILD)/%.o: $(SOURCE)/%.s
	$(call Q,AS,$@)$(AS) $< $(OUTPUT_OPTION)

$(BUILD)/%.o: $(BUILD)/%.bin
	$(call Q,OBJCOPY,$@)cd $(BUILD); $(OBJCOPY) -Ibinary -Oelf32-littlearm -Barmv5te $(subst $(BUILD)/,,$< $@)

%_ascii.bin: %.bin
	$(call Q,DD,$@)dd if=$< of=$@ count=8

$(BUILD)/font.bin: unifont-8.0.01.bmp
	$(call Q,CONVERT,$@)convert -crop +32+64 -transpose +repage -crop 16 -append $< - | convert -flop - mono:$@;

$(foreach f,$(NEED_DIRS),$(eval $f : | $(dir $f)D))

%/D:
	$(call Q,MKDIR,$(dir $@))mkdir -p $(dir $@)

clean:
	$(call Q,RM,)rm -fr $(BUILD)

-include $(DEPS)
