SDIR := $(SRC_DIR)/kernel/arch/$(ARCH_DIR)
BDIR := $(BUILD_DIR)/kernel/arch/$(ARCH_DIR)

SRC_FILES := $(shell cd $(SDIR) && find -L * -type f | LC_ALL=C sort)
CFILES := $(filter %.c,$(SRC_FILES))
ASFILES := $(filter %.s,$(SRC_FILES))

OBJS := $(addprefix $(BDIR)/, $(CFILES:.c=.c.o) $(ASFILES:.s=.s.o))
HEADER_DEPS := $(addprefix $(BDIR)/, $(CFILES:.c=.c.d) $(ASFILES:.s=.s.d))

all: $(BDIR)/$(ARCH).a

$(BDIR)/$(ARCH).a: $(OBJS)
	ar rcs $@ $(OBJS)
	ranlib $@

$(BDIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BDIR)/%.s.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BDIR)/%.c.d: $(SDIR)/%.c $(OBJS)
	$(CC) $(CFLAGS) -MM $< -MF $@

$(BDIR)/%.s.d: $(SDIR)/%.s $(OBJS)
	$(CC) $(ASFLAGS) -MM $< -MF $@