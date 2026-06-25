export TOP_DIR := $(CURDIR)
export SRC_DIR := $(TOP_DIR)/src
export BUILD_DIR := $(TOP_DIR)/build
CONFIG_FILE := .config

export CC := gcc
export LD := ld

export CFLAGS := -ffreestanding -nostdlib -Wall -Wextra \
          -I$(TOP_DIR)/include -I$(BUILD_DIR)/include

export LDFLAGS := -nostdlib

all: kernel

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

SUBDIR_OBJS := $(addprefix $(BUILD_DIR)/,$(addsuffix /built-in.o,$(subdir-y)))

.PHONY: kernel
kernel: $(BUILD_DIR)/tachyon

$(BUILD_DIR)/tachyon: $(SUBDIR_OBJS) include/generated/autoconf.h linker.lds
	$(LD) $(LDFLAGS) $(SUBDIR_OBJS) -o $(BUILD_DIR)/tachyon -T linker.lds

$(SUBDIR_OBJS): include/generated/autoconf.h
	$(MAKE) -C $(patsubst $(BUILD_DIR)/%,%,$(dir $@))

.PHONY: clean
clean:
	-rm -rf build

.PHONY: cleanall
cleanall: clean
	-rm .config*