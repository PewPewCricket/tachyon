export

VERSION = 0.1.0

PDIR := $(SRCDIR)/kernel

CFLAGS += \
	-I. \
	-I$(PDIR)/arch/$(ARCH_DIR)/include \
	-I$(PDIR)/include \
	-I$(PDIR)

ASFLAGS += \
	-I. \
	-I$(PDIR)/arch/$(ARCH_DIR)/include \
	-I$(PDIR)/include \
	-I$(PDIR)

SRCFILES := $(shell cd $(PDIR) && find -L * -type f | grep -v "arch" | LC_ALL=C sort)
CFILES := $(filter %.c,$(SRCFILES))
OBJ := $(CFILES:.c=.c.o)
ARCH_OBJ := $(shell cd $(PDIR) && find -L arch/$(ARCH_DIR) -name "*.o" -type f | LC_ALL=C sort)
HEADER_DEPS := $(CFILES:.c=.c.d)
OUTPUT := kernel-$(VERSION)-$(ARCH)

all: $(OUTPUT)

$(OUTPUT): arch/ $(OBJ) 
	@$(eval ARCH_OBJ=$(shell cd $(PDIR) && find -L arch/$(ARCH_DIR) -name "*.o" -type f | LC_ALL=C sort))
	@echo "  LD      " $@
	@$(LD) $(LDFLAGS) $(OBJ) $(ARCH_OBJ) -o $@ -T $(PDIR)/arch/$(ARCH_DIR)/linker.ld

%.c.o: %.c
	@echo "  CC      " $<
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: arch/
arch/: $(OBJ) arch/$(ARCH_DIR)/Makefile
	@echo "  MK      " kernel/$@/$(ARCH_DIR)
	@$(MAKE) -C $@/$(ARCH_DIR) $(param)

.PHONY: install
install: all
	@echo "Installing kernel-$(VERSION)-$(ARCH) into $(DESTDIR)/boot/"
	@mkdir -p $(DESTDIR)/boot
	@cp kernel-$(VERSION)-$(ARCH) $(DESTDIR)/boot/kernel

.PHONY: clean
clean: param = clean
clean: $(OBJ) arch/
	@echo "  RM      " $(OBJ) $(HEADER_DEPS)
	@rm -r $(OBJ) $(shell cd $(PDIR) && find -L * -name "*.d" -type f | LC_ALL=C sort)

-include $(HEADER_DEPS)