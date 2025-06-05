CC := i686-elf-gcc

CFLAGS += \
		-nostdlib -ffreestanding -m32 \
		-Og -g -Wall -Wextra -Werror \
		-Wno-unused -Wno-unused-parameter -march=i386 \
		-funsigned-bitfields -fuse-ld=bfd

AS := $(CC)
ASFLAGS := $(CFLAGS)
LD := i686-elf-ld
LDFLAGS := -nostdlib -melf_i386

ARCH=i386
ARCH_DIR=i386