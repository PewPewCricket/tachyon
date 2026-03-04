export TOPDIR := $(CURDIR)
export BUILD_DIR := $(CURDIR)/build
CONFIG_FILE := .config

export CC := gcc
export LD := ld

export CFLAGS := -ffreestanding -nostdlib -Wall -Wextra \
          -I$(TOPDIR)/include -I$(BUILD_DIR)/include

export LDFLAGS := -nostdlib

# Kconfig stuff

KCONFIG_MCONF := python scripts/kconfig/menuconfig.py
KCONFIG_GENCONFIG := python scripts/kconfig/genconfig.py

.PHONY: menuconfig
menuconfig: scripts/kconfig/menuconfig.py
	$(KCONFIG_MCONF) Kconfig

include/generated/autoconf.h: $(CONFIG_FILE)
	mkdir -p $(BUILD_DIR)/include/generated
	$(KCONFIG_GENCONFIG) --config-out $(BUILD_DIR)/autoconf.mk --header-path $(BUILD_DIR)/include/generated/autoconf.h

-include $(BUILD_DIR)/autoconf.mk

# Kbuild ripoff

obj-y :=
subdir-y :=

include Kbuild

SUBDIR_OBJS := $(addprefix $(BUILD_DIR)/,$(addsuffix /built-in.a,$(subdir-y)))

.PHONY: kernel
kernel: $(BUILD_DIR)/tachyon

$(BUILD_DIR)/tachyon: $(SUBDIR_OBJS) include/generated/autoconf.h linker.lds
	$(LD) $(LDFLAGS) $(SUBDIR_OBJS) -o $(BUILD_DIR)/tachyon -T linker.lds

$(SUBDIR_OBJS): include/generated/autoconf.h
	$(MAKE) -C $(patsubst $(BUILD_DIR)/%,%,$(dir $@))

.PHONY: iso
iso: $(BUILD_DIR)/tachyon
	cd $(BUILD_DIR)
	-git clone https://codeberg.org/Limine/Limine.git limine --branch=v10.x-binary --depth=1
	make -C limine
	mkdir -p iso_root/boot
	cp -v $(BUILD_DIR)/tachyon iso_root/boot/
	mkdir -p iso_root/boot/limine
	cp -v $(TOPDIR)/sysroot/boot/limine/limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin \
	limine/limine-uefi-cd.bin iso_root/boot/limine/
	mkdir -p iso_root/EFI/BOOT
	cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/
	cp -v limine/BOOTIA32.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
            -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
            -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
            -efi-boot-part --efi-boot-image --protective-msdos-label \
            iso_root -o image.iso
	./limine/limine bios-install image.iso

.PHONY: clean
clean:
	rm -rf build image.iso iso_root limine