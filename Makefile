export

SRC_DIR := $(shell pwd)
BUILD_DIR := $(shell pwd)/build

ifeq ($(CONFIG),)
CONFIG := i386
endif

include $(SRC_DIR)/configs/$(CONFIG).mk

.PHONY: all clean
all: $(BUILD_DIR)/kernel.img

$(BUILD_DIR)/kernel.img: kernel-build


kernel-build: libk-build
	$(MAKE) -C kernel

libk-build:
	$(MAKE) -C libk

clean:
	-@rm -r $(BUILD_DIR)/*