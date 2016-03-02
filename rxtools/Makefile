# Copyright (C) 2015-2016 The PASTA Team
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

all: $(BUILD)/code.bin $(BUILD)/bin/tool.bin

include $(RXTOOLSMK)

LD := $(CC)

INCDIR += -Isource/include
CPPDEFS	:= -DSYS_PATH=\"$(SYS_PATH)\" -DPATCHES_PATH=\"$(PATCHES_PATH)\"
CPPFLAGS := $(INCDIR) $(CPPDEFS)

CFORCE := -fomit-frame-pointer -ffast-math
CMACHINE := -mtune=arm946e-s
CWARNING := -Wall -Wno-multichar
CFLAGS := -g -O2 -std=c99 $(CWARNING) $(CFORCE) $(CMACHINE) $(CPPFLAGS) -DVERSION=\"$(GIT_VERSION)\"

LDFLAGS := -nostartfiles -g $(CMACHINE)

INIT_CCOBJS := $(addprefix $(BUILD)/init/,cstart.o	\
	lib/crypto.o lib/draw.o lib/fs.o	\
	lib/fatfs/diskio.o lib/fatfs/ff.o lib/fatfs/option/unicode.o 	\
	lib/media/nand.o lib/media/tmio/tmio.o)

INIT_OBJS := $(INIT_CCOBJS)	\
	$(BUILD)/init/lib/delay.o $(BUILD)/init/start.o $(BUILD)/font_ascii.o

TOOL_CCOBJS := $(addprefix $(BUILD)/tool/,cstart.o features/decryption.o	\
	features/install.o features/firm.o features/screenshot.o	\
	lib/polarssl/padlock.o lib/polarssl/sha2.o lib/polarssl/aes.o	\
	lib/jsmn/jsmn.o lib/cfg.o lib/console.o lib/hid.o	\
	lib/lang.o lib/log.o lib/i2c.o lib/ncch.o)

TOOL_OBJS := $(TOOL_CCOBJS)

DEPS	:= $(subst $(BUILD)/,$(DEPS_DIR)/,$(TOOL_CCOBJS) $(INIT_CCOBJS))

$(call DEPDIR,$(BUILD)/bin/init.elf $(BUILD)/bin/tool.elf $(BUILD)/font.bin $(INIT_OBJS) $(TOOL_OBJS) $(DEPS))

MAKEDEP	= @$(COMPILE.c) -MM $< > $(patsubst $(BUILD)/%.o,$(DEPS_DIR)/%.d,$@)

.PHONY: $(BUILD)/code.bin all clean

$(BUILD)/code.bin: $(BUILD)/bin/init.bin $(BUILD)/CakeHax/D
	$(call Q,MAKE,CakeHax)$(MAKE) dir_out=../$(dir $@) dir_build=../$(BUILD)/CakeHax name=$(notdir $@) filepath=$(SYS_PATH)/ -C CakeHax
	dd if=$< of=$@ bs=512 seek=144

%.bin: %.elf
	$(call Q,OBJCOPY,$@)$(OBJCOPY) -O binary $< $@

$(BUILD)/bin/init.elf: $(INIT_OBJS)
	$(call Q,LINK,$@)$(LINK.o) -Tinit.ld $^ $(OUTPUT_OPTION)

# Use Thumb libraries because sbrk of ARM newlib in devkitPro doesn't work.
$(BUILD)/bin/tool.elf: $(BUILD)/bin/init.elf $(TOOL_OBJS)
	$(call Q,LINK,$@)$(LINK.o) -mthumb -mthumb-interwork -Wl,-Ttool.ld,-R$< $(TOOL_OBJS) $(OUTPUT_OPTION)

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
