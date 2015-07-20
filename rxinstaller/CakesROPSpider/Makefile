export PATH	:= $(DEVKITARM)/bin:$(PATH)

CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld

INCLUDES=-I../include
DEFINES=

ARCH=-march=armv6k -mtune=mpcore -mlittle-endian
CFLAGS=-fshort-wchar -fomit-frame-pointer -ffast-math -std=gnu99 -Os -ffunction-sections -g -mword-relocations $(ARCH) $(INCLUDES) $(DEFINES)
LDFLAGS=-nostartfiles --specs=crs.specs -Wl,-gc-sections $(ARCH)
OBJCOPY=arm-none-eabi-objcopy
OCFLAGS=-R .compat

OBJS=$(patsubst src/%.c, obj/%.o, $(wildcard src/*.c))
OBJS+=$(patsubst src/%.s, obj/%.o, $(wildcard src/*.s))
OBJS+=$(patsubst src/%.S, obj/%.o, $(wildcard src/*.S))

OUT_DIR=obj

all: code.bin

obj/%.o: src/%.c | dirs
	$(CC) -c -o $@ $< $(CFLAGS)

obj/%.o: src/%.s | dirs
	$(CC) -x assembler-with-cpp $(ARCH) -c $^ -o $@

code.elf: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

code.bin: code.elf
	$(OBJCOPY) $(OCFLAGS) -O binary $^ $@

dirs: ${OUT_DIR}

${OUT_DIR}:
	mkdir -p ${OUT_DIR}

.PHONY: clean

clean:
	rm -rf *~ obj *.elf *.bin
