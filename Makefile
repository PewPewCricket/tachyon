MAKEFLAGS += -rR
.SUFFIXES:

PDIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BDIR ?= $(PDIR)/build

CROSS_32 := $(PDIR)/cross/i686
CC32 := $(CROSS_32)/bin/i686-elf-gcc

CROSS_64 := $(PDIR)/cross/amd64
CC64 := $(CROSS_64)/bin/x86_64-elf-gcc

CPPFLAGS += -I$(PDIR)/include
CFLAGS += -std=c23 -Wall -Wextra -ffreestanding \
          -fstack-protector-all -mabi=sysv \
          -nostartfiles -MMD -MP \
          -nostdlib -static \
          -Wl,--build-id=none,--orphan-handling=warn

.PHONY: all debug iso install clean clean-all toolchain toolchain-offline

export PDIR BDIR CC32 CC64 CFLAGS CPPFLAGS

all: Makefile toolchain
	$(MAKE) -C stub release TYPE=release
	$(MAKE) -C kernel release TYPE=release

debug: Makefile toolchain
	$(MAKE) -C stub debug TYPE=debug
	$(MAKE) -C kernel debug TYPE=debug

iso:
	mkdir -p $(BDIR)/iso/boot/grub
	cp $(BDIR)/stub/stub.elf $(BDIR)/iso/boot/stub.elf
	cp $(BDIR)/kernel/kernel.elf $(BDIR)/iso/boot/kernel.elf
	cp stub/grub.cfg $(BDIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $(BDIR)/os.iso $(BDIR)/iso
	qemu-system-x86_64 --cdrom $(BDIR)/os.iso -serial stdio

toolchain:
	if [ ! -d cross ]; then env -i PATH=/usr/bin:/bin bash $(PDIR)/toolchain.sh $(PDIR) online; fi

toolchain-offline:
	env -i PATH=/usr/bin:/bin bash $(PDIR)/toolchain.sh $(PDIR) offline

install:
	@echo "Install target not implemented"

clean:
	rm -rf $(BDIR)

clean-all:
	rm -rf $(BDIR) $(PDIR)/cross
