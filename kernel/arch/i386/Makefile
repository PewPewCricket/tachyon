SRCFILES := $(shell cd $(PDIR)/arch/$(ARCH_DIR) && find -L * -type f | grep -v "arch" | LC_ALL=C sort)
CFILES := $(filter %.c,$(SRCFILES))
ASFILES := $(filter %.s,$(SRCFILES))
OBJ := $(CFILES:.c=.c.o) $(ASFILES:.s=.s.o)
HEADER_DEPS := $(CFILES:.c=.c.d) $(ASFILES:.s=.s.d)

all: $(OBJ)

%.c.o: %.c
	@echo "  CC      " $<
	@$(CC) $(CFLAGS) -c $< -o $@

%.s.o: %.s
	@echo "  AS      " $<
	@$(AS) $(ASFLAGS) -c $< -o $@

.PHONY: clean
clean: $(OBJ)
	@echo "  RM      " $(OBJ) $(HEADER_DEPS)
	@rm -r $(OBJ) $(shell cd $(PDIR)/arch/$(ARCH_DIR) && find -L * -name "*.d" -type f | LC_ALL=C sort)

-include $(HEADER_DEPS)