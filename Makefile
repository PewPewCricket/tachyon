export TOP_DIR := $(CURDIR)
export SRC_DIR := $(TOP_DIR)/src
export BUILD_DIR := $(TOP_DIR)/build
CONFIG_FILE := .config

export CC := gcc
export LD := ld

export CFLAGS := -ffreestanding -nostdlib -Wall -Wextra \
          -I$(TOP_DIR)/include -I$(BUILD_DIR)/include

export LDFLAGS := -nostdlib

KCONFIG_MCONF := python3 scripts/kconfig/menuconfig.py
KCONFIG_GENCONFIG := python3 scripts/kconfig/genconfig.py

# Rules for autoconf files
ifeq ($(filter clean cleanall menuconfig,$(MAKECMDGOALS)),)

$(BUILD_DIR)/autoconf.mk: $(BUILD_DIR)/include/generated/autoconf.h
$(BUILD_DIR)/include/generated/autoconf.h: $(CONFIG_FILE)
	mkdir -p $(BUILD_DIR)/include/generated
	$(KCONFIG_GENCONFIG) --config-out $(BUILD_DIR)/autoconf.mk --header-path $(BUILD_DIR)/include/generated/autoconf.h

include $(BUILD_DIR)/autoconf.mk

endif

# Kbuild ripoff v2
obj-y :=
subdir-y :=
export toolchain-y :=

-include Kbuild

SUBDIR_OBJS := $(addprefix $(BUILD_DIR)/,$(addsuffix /built-in.o,$(subdir-y)))
export TOOL_SRC := $(TOP_DIR)/cross/src
export TOOL_BUILD := $(TOP_DIR)/cross/build/$(toolchain-y)
export TOOL_DIR := $(TOP_DIR)/cross/$(toolchain-y)

-include scripts/toolchain.mk

# Targets
all: kernel

.PHONY: menuconfig
menuconfig: scripts/kconfig/menuconfig.py
	$(KCONFIG_MCONF) Kconfig
	# I have to do this because make fucking sucks
	mkdir -p $(BUILD_DIR)/include/generated
	$(KCONFIG_GENCONFIG) --config-out $(BUILD_DIR)/autoconf.mk --header-path $(BUILD_DIR)/include/generated/autoconf.h

$(CONFIG_FILE):
	@echo "No configuration file found. Please run 'make menuconfig' first."
	@exit 1

.PHONY: toolchain
toolchain: $(TOOL_DIR)/bin/$(toolchain-y)-elf-gcc $(TOOL_DIR)/bin/$(toolchain-y)-elf-gdb $(TOOL_DIR)/bin/$(toolchain-y)-elf-ld $(CONFIG_FILE)

.PHONY: kernel
kernel: $(BUILD_DIR)/tachyon

$(BUILD_DIR)/tachyon: $(SUBDIR_OBJS) $(BUILD_DIR)/include/generated/autoconf.h $(TOP_DIR)/linker.lds $(TOOL_DIR)/bin/$(toolchain-y)-elf-gcc $(TOOL_DIR)/bin/$(toolchain-y)-elf-gdb $(TOOL_DIR)/bin/$(toolchain-y)-elf-ld
	$(LD) $(LDFLAGS) $(SUBDIR_OBJS) -o $(BUILD_DIR)/tachyon -T $(TOP_DIR)/linker.lds

$(SUBDIR_OBJS): $(BUILD_DIR)/include/generated/autoconf.h
	$(MAKE) -C $(patsubst $(BUILD_DIR)/%,%,$(dir $@))

.PHONY: clean
clean:
	-rm -rf build

.PHONY: cleanall
cleanall: clean
	-rm .config*
	-rm -rf cross