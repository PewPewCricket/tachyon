MAKEFLAGS += -rR
.SUFFIXES:

override OUTPUT := nerve

CC := cc
CFLAGS := -g -Og -pipe
CPPFLAGS :=
NASMFLAGS := -F dwarf -g
LDFLAGS :=

override CC_IS_CLANG := $(shell ! $(CC) --version 2>/dev/null | grep 'clang' >/dev/null 2>&1; echo $$?)

ifeq ($(CC_IS_CLANG),1)
    override CC += \
        -target i686-unknown-none
endif

override CFLAGS += \
    -Wall \
    -Wextra \
    -Werror \
    -std=gnu11 \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-PIC \
    -m32 \
    -march=i686 \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone

override CPPFLAGS := \
    -I src \
    -I src/libk \
    $(CPPFLAGS) \
    -MMD \
    -MP

override NASMFLAGS += \
    -Wall \
    -f elf32 \

override LDFLAGS += \
    -Wl,-m,elf_i386 \
    -Wl,--build-id=none \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    -T linker.ld

override SRCFILES := $(shell cd src && find -L * -type f | LC_ALL=C sort)
override CFILES := $(filter %.c,$(SRCFILES))
override ASFILES := $(filter %.S,$(SRCFILES))
override NASMFILES := $(filter %.asm,$(SRCFILES))
override OBJ := $(addprefix build/obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
override HEADER_DEPS := $(addprefix build/obj/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

.PHONY: all clean iso run debug
all: build/bin/$(OUTPUT)

-include $(HEADER_DEPS)

build/bin/$(OUTPUT): GNUmakefile linker.ld $(OBJ)
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@

build/obj/%.c.o: src/%.c GNUmakefile
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

build/obj/%.S.o: src/%.S GNUmakefile
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

build/obj/%.asm.o: src/%.asm GNUmakefile
	mkdir -p "$$(dirname $@)"
	nasm $(NASMFLAGS) $< -o $@

clean:
	rm -rf build/*
	rm os.iso

iso:
	mkdir -p build/iso/boot/grub
	cp build/bin/$(OUTPUT) build/iso/boot/$(OUTPUT)
	cp grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -o os.iso build/iso

run:
	qemu-system-i386 --cdrom os.iso

debug:
	./scripts/debug.sh $(OUTPUT)