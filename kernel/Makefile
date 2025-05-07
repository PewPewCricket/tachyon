export

VERSION = 0.1.0

PDIR := $(SRCDIR)/kernel

SRCFILES := $(shell cd $(PDIR) && find -L * -type f | grep -v "arch" | LC_ALL=C sort)
CFILES := $(filter %.c,$(SRCFILES))
OBJ := $(CFILES:.c=.c.o)
ARCH_OBJ := $(PDIR)/arch/$(ARCH_DIR)/all.a
HEADER_DEPS := $(CFILES:.c=.c.d)
OUTPUT := kernel-$(VERSION)-$(ARCH)

all: $(OUTPUT)

$(OUTPUT): $(OBJ) $(ARCH_OBJ)
	@echo "  LD      " $@
	@$(LD) $(LDFLAGS) $(OBJ) $(ARCH_OBJ) $(SRCDIR)/libc/libk.a -o $@ -T $(PDIR)/arch/$(ARCH_DIR)/linker.ld

%.c.o: %.c
	@echo "  CC      " $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(ARCH_OBJ): arch/$(ARCH_DIR)/Makefile
	@echo "  MK      " kernel/arch/$(ARCH_DIR)
	@$(MAKE) -C $(PDIR)/arch/$(ARCH_DIR)



.PHONY: install
install: all
	@echo "Installing kernel-$(VERSION)-$(ARCH) into $(DESTDIR)/boot/"
	@mkdir -p $(DESTDIR)/boot
	@cp kernel-$(VERSION)-$(ARCH) $(DESTDIR)/boot/kernel

.PHONY: clean
clean:
	@echo "  RM      " $(OBJ) $(HEADER_DEPS) $(OUTPUT)
	-@rm -rf $(OBJ) $(HEADER_DEPS) $(OUTPUT)
	@echo "  MK      " kernel/arch/$(ARCH_DIR)
	@$(MAKE) -C $(PDIR)/arch/$(ARCH_DIR) clean

-include $(HEADER_DEPS)