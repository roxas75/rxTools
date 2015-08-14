export PATH	:= $(DEVKITARM)/bin:$(PATH)

ifndef CODE_PATH
$(error "Please set CODE_PATH")
endif

AS	:= arm-none-eabi-as
CC	:= arm-none-eabi-gcc
LD	:= arm-none-eabi-ld
OC	:= arm-none-eabi-objcopy

CFLAGS	:= -fshort-wchar -fomit-frame-pointer -std=gnu99 -Os -g -march=armv6k -mtune=mpcore -I../include -I../../include -DCODE_PATH=\"$(CODE_PATH)\"
LDFLAGS	:= -T crs.ld
OCFLAGS	:= -R .compat

BUILD	:= $(CURDIR)/build
SOURCE	:= $(CURDIR)/source
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
