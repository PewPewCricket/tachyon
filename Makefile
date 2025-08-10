MAKEFLAGS += -rR
.SUFFIXES:

OUTPUT := kernel.elf

CC := cc
CFLAGS := -pipe
CPPFLAGS :=
NASMFLAGS := -F dwarf -g
LDFLAGS :=

CC_IS_CLANG := $(shell ! $(CC) --version 2>/dev/null | grep 'clang' >/dev/null 2>&1; echo $$?)

ifeq ($(CC_IS_CLANG),1)
    CC += -target i686-unknown-none
endif

BASE_CFLAGS := \
    -Wall \
    -Wextra \
    -std=c23 \
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

CPPFLAGS := \
    -Iinclude \
    $(CPPFLAGS) \
    -MMD \
    -MP

NASMFLAGS += \
    -Wall \
    -f elf32

LDFLAGS += \
    -Wl,-m,elf_i386 \
    -Wl,--build-id=none \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    -T linker.ld

SRCFILES := $(shell find -L * -type f | LC_ALL=C sort)
CFILES := $(filter %.c,$(SRCFILES))
ASFILES := $(filter %.S,$(SRCFILES))
NASMFILES := $(filter %.asm,$(SRCFILES))

.PHONY: all clean iso release debug release-iso debug-iso

all: release-iso

clean:
	rm -rf build

# === RELEASE BUILD ===

RELEASE_BUILD_DIR := build/release
RELEASE_OBJ = $(addprefix $(RELEASE_BUILD_DIR)/obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
RELEASE_DEPS = $(addprefix $(RELEASE_BUILD_DIR)/obj/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

-include $(RELEASE_DEPS)

release: CFLAGS := $(BASE_CFLAGS) -O3
release: $(RELEASE_BUILD_DIR)/bin/$(OUTPUT)

$(RELEASE_BUILD_DIR)/bin/$(OUTPUT): linker.ld $(RELEASE_OBJ)
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(LDFLAGS) $(RELEASE_OBJ) -o $@

$(RELEASE_BUILD_DIR)/obj/%.c.o: %.c
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(RELEASE_BUILD_DIR)/obj/%.S.o: %.S
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(RELEASE_BUILD_DIR)/obj/%.asm.o: %.asm
	mkdir -p "$$(dirname $@)"
	nasm $(NASMFLAGS) $< -o $@

# === DEBUG BUILD ===

DEBUG_BUILD_DIR := build/debug
DEBUG_OBJ = $(addprefix $(DEBUG_BUILD_DIR)/obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
DEBUG_DEPS = $(addprefix $(DEBUG_BUILD_DIR)/obj/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

-include $(DEBUG_DEPS)

debug: CFLAGS := $(BASE_CFLAGS) -Og -g -DKDEBUG
debug: $(DEBUG_BUILD_DIR)/bin/$(OUTPUT)

$(DEBUG_BUILD_DIR)/bin/$(OUTPUT): linker.ld $(DEBUG_OBJ)
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(LDFLAGS) $(DEBUG_OBJ) -o $@

$(DEBUG_BUILD_DIR)/obj/%.c.o: %.c
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(DEBUG_BUILD_DIR)/obj/%.S.o: %.S
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(DEBUG_BUILD_DIR)/obj/%.asm.o: %.asm
	mkdir -p "$$(dirname $@)"
	nasm $(NASMFLAGS) $< -o $@

# === ISO targets ===

iso: release-iso

release-iso: release
	mkdir -p build/iso/boot/grub
	cp $(RELEASE_BUILD_DIR)/bin/$(OUTPUT) build/iso/boot/$(OUTPUT)
	cp grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -o build/os.iso build/iso
	qemu-system-i386 --cdrom build/os.iso -serial stdio

debug-iso: debug
	mkdir -p build/iso/boot/grub
	cp $(DEBUG_BUILD_DIR)/bin/$(OUTPUT) build/iso/boot/$(OUTPUT)
	cp grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -o build/os.iso build/iso
	qemu-system-i386 --cdrom build/os.iso -serial stdio
