export PATH	:= $(DEVKITARM)/bin:$(PATH)

CODE_PATH	:= rxTools/system/code.bin

AS	:= arm-none-eabi-as
CC	:= arm-none-eabi-gcc
LD	:= arm-none-eabi-ld
OC	:= arm-none-eabi-objcopy

INCDIR	+= -I../include
CFLAGS	:= -fshort-wchar -fomit-frame-pointer -std=gnu99 -Os -g -march=armv6k -mtune=mpcore $(INCDIR) -DCODE_PATH=\"$(CODE_PATH)\"
LDFLAGS	:= -T crs.ld
OCFLAGS	:= -R .compat

BUILD	:= build
SOURCE	:= source
OBJS	:= $(patsubst $(SOURCE)/%.c, $(BUILD)/%.o, $(wildcard $(SOURCE)/*.c))	\
	$(patsubst $(SOURCE)/%.s, $(BUILD)/%.o, $(wildcard $(SOURCE)/*.s))

all: code.bin

$(BUILD)/%.o: $(SOURCE)/%.c $(BUILD)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD)/%.o: $(SOURCE)/%.s $(BUILD)
	$(AS) $(ARCH) -c $< -o $@

code.elf: $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)

code.bin: code.elf
	$(OC) $(OCFLAGS) -O binary $^ $@

$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean

clean:
	rm -rf *~ $(BUILD) *.elf *.bin
