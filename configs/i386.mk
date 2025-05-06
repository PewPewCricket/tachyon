CC := i686-elf-gcc

CFLAGS += \
		-nostdlib -ffreestanding -m32 \
		-O3 -Wall -Wextra -Werror \
		-Wno-unused -Wno-unused-parameter -march=i386 \
		-funsigned-bitfields -fuse-ld=bfd -masm=intel

AS := $(CC)
ASFLAGS := $(CFLAGS) -Wa,-msyntax=intel
LD := i686-elf-ld
LDFLAGS := -nostdlib -melf_i386

ARCH=i386
ARCH_DIR=i386
ARCH_I386=y
ARCH_BITS=32

X86_MULTIBOOT=y

