MAKEFLAGS += -rR
.SUFFIXES:

PDIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BDIR ?= $(PDIR)/build

CPPFLAGS += -I$(PDIR)/include
CFLAGS += -std=c23 -Wall -Wextra -ffreestanding \
          -fno-stack-protector -fno-stack-check \
          -nostdlib -fno-pie -fno-pic -MMD -MP \
          -static \
          -Wl,--build-id=none,--orphan-handling=warn,-no-pie

export PDIR BDIR CPPFLAGS CFLAGS

.PHONY: all debug iso install clean

all: Makefile
	$(MAKE) -C stub release TYPE=release
	$(MAKE) -C kernel release TYPE=release

debug: Makefile
	$(MAKE) -C stub debug TYPE=debug
	$(MAKE) -C kernel debug TYPE=debug

iso:
	mkdir -p $(BDIR)/iso/boot/grub
	cp $(BDIR)/stub/stub.elf $(BDIR)/iso/boot/stub.elf
	cp $(BDIR)/kernel/kernel.elf $(BDIR)/iso/boot/kernel.elf
	cp stub/grub.cfg $(BDIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $(BDIR)/os.iso $(BDIR)/iso
	qemu-system-i386 --cdrom $(BDIR)/os.iso -serial stdio

install:
	@echo "Install target not implemented"

clean:
	rm -rf $(BDIR)