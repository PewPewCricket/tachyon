# Top level Makefile
#
# Targets:
# 	build-kernel
# 	install-kernel
# 	clean-kernel
#
# 	build-all
#	install-all
#	clean-all

export

ifeq ($(SRCDIR),)
SRCDIR := $(shell pwd)
endif

ifeq ($(BUILDDIR),)
BUILDDIR := $(SRCDIR)
endif

ifeq ($(CONFIG),)
CONFIG := i386
endif

include $(SRCDIR)/configs/$(CONFIG).mk

MAKEFLAGS += --no-print-directory

GRUB_MKRESCUE = $(shell command -v grub2-mkrescue 2> /dev/null)
ifeq ($(GRUB_MKRESCUE),)
GRUB_MKRESCUE = grub-mkrescue
endif

DESTDIR = $(BUILDDIR)/sysroot

# Targets for everything
.PHONY: build-all install-all clean-all
build-all: build-kernel
install-all: install-kernel
clean-all: clean-kernel
	-@rm -r iso sysroot serve-os.iso

# Kernel Targets
.PHONY: build-kernel clean-kernel install-kernel
build-kernel:
	$(MAKE) -C $(SRCDIR)/kernel/

install-kernel:
	$(MAKE) -C $(SRCDIR)/kernel/ install

clean-kernel:
	$(MAKE) -C $(SRCDIR)/kernel/ clean

# Kernel ISO assembly
.PHONY: try
try: nerve-os.iso
	qemu-system-i386 -cdrom nerve-os.iso -serial stdio -m 2G -d cpu_reset -no-reboot -boot d

nerve-os.iso: iso/boot/kernel
	$(GRUB_MKRESCUE) -d /usr/lib/grub/i386-pc/ -o nerve-os.iso iso/

iso/boot/kernel:
	mkdir -p "$$(dirname $@)"
	cp $(SRCDIR)/kernel/kernel-*-* iso/boot/kernel
	cp -r $(SRCDIR)/loader/grub2/* iso/boot/