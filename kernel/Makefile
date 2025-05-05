HOSTARCH 				?= i386

CFLAGS					?= -O2 -g
CPPFLAGS				?=
LDFLAGS					?=
LIBS					?=

DESTDIR					?=
PREFIX					?= /usr/local
BOOTDIR					?= /boot
INCLUDEDIR				?= $(PREFIX)/include

CFLAGS					:= $(CFLAGS) -ffreestanding -Wall -Wextra
CPPFLAGS				:= $(CPPFLAGS) -D__is_kernel -Iinclude
LDFLAGS					:= $(LDFLAGS)
LIBS					:= $(LIBS) -nostdlib -lgcc

ARCHDIR					= arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

override OUTPUT			:= kernel.elf
override SRCFILES		:= $(shell find -L kernel/* -type f | LC_ALL=C sort)
override ARCHFILES		:= $(shell find -L $(ARCHDIR)/* -type f | LC_ALL=C sort)
override CFILES			:= $(filter %.c,$(SRCFILES))
override ASFILES	    := $(filter %.s,$(ARCHFILES))
override OBJS			:= $(CFILES:.c=.c.o) $(ASFILES:.s=.s.o)
override HEADER_DEPS	:= $(CFILES:.c=.c.d) $(ASFILES:.s=.s.d)

.PHONY: all clean install install-headers install-kernel
all: $(OUTPUT)

$(OUTPUT): $(ARCHDIR)/linker.ld $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $(OBJS) -o $@

kernel/%.c.o: kernel/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
	
$(ARCHDIR)/%.s.o: $(ARCHDIR)/%.s
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OUTPUT) $(OBJS) $(HEADER_DEPS)

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: $(OUTPUT)
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp $(OUTPUT) $(DESTDIR)$(BOOTDIR)

-include $(HEADER_DEPS)