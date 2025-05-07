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

CFLAGS += \
	-I. \
	-I$(PDIR)/arch/$(ARCH_DIR)/include \
	-I$(PDIR)/include \
	-I$(PDIR) \
	-Ilibc/include \
	-MD

ASFLAGS += \
	-I. \
	-I$(PDIR)/arch/$(ARCH_DIR)/include \
	-I$(PDIR)/include \
	-I$(PDIR)

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
.PHONY: install clean
all: build-kernel build-libc
install: install-kernel install-libc
clean: clean-kernel clean-libc
	-@rm -r iso sysroot serve-os.iso

# Kernel Targets
.PHONY: build-kernel clean-kernel install-kernel
build-kernel: build-libc
	@echo "  MK      " kernel/
	@$(MAKE) -C $(SRCDIR)/kernel/

install-kernel: 
	@echo "  MK      " kernel/
	@$(MAKE) -C $(SRCDIR)/kernel/ install

clean-kernel:
	@echo "  MK      " kernel/
	@$(MAKE) -C $(SRCDIR)/kernel/ clean

# Libc Targets
.PHONY: build-libc clean-libc install-libc
build-libc:
	@echo "  MK      " libc/
	@$(MAKE) -C $(SRCDIR)/libc/

install-libc:
	@echo "  MK      " libc/
	@$(MAKE) -C $(SRCDIR)/libc/ install

clean-libc:
	@echo "  MK      " libc/
	@$(MAKE) -C $(SRCDIR)/libc/ clean

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