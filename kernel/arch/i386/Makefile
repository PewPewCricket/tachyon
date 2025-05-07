SRCFILES := $(shell cd $(PDIR)/arch/$(ARCH_DIR) && find -L * -type f | grep -v "arch" | LC_ALL=C sort)
CFILES := $(filter %.c,$(SRCFILES))
ASFILES := $(filter %.s,$(SRCFILES))
OBJ := $(CFILES:.c=.c.o) $(ASFILES:.s=.s.o)
HEADER_DEPS := $(CFILES:.c=.c.d) $(ASFILES:.s=.s.d)

all: all.a

all.a: $(OBJ)
	@echo "  AR      " $@
	@ar rcs $@ $(OBJ)
	@ranlib $@

%.c.o: %.c
	@echo "  CC      " $<
	@$(CC) $(CFLAGS) -c $< -o $@

%.s.o: %.s
	@echo "  AS      " $<
	@$(AS) $(ASFLAGS) -Wa,-MD=$<.d -c $< -o $@

.PHONY: clean
clean: $(OBJ) all.a
	@echo "  RM      " $(OBJ) $(HEADER_DEPS) all.a
	-@rm -rf $(OBJ) all.a $(HEADER_DEPS)

-include $(HEADER_DEPS)