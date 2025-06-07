export

SDIR := $(SRC_DIR)/kernel
BDIR := $(BUILD_DIR)/kernel

SRC_FILES := $(shell cd $(SDIR) && find -L * -type f | grep -v "arch/" | LC_ALL=C sort)
SRC_FILES += $(addprefix arch/$(ARCH_DIR)/, $(shell cd $(SDIR)/arch/$(ARCH_DIR) && find -L * -type f | LC_ALL=C sort))
CFILES := $(filter %.c,$(SRC_FILES))
ASFILES := $(filter %.s,$(SRC_FILES))

OBJS := $(addprefix $(BDIR)/, $(CFILES:.c=.c.o) $(ASFILES:.s=.s.o))
HEADER_DEPS := $(addprefix $(BDIR)/, $(CFILES:.c=.c.d))

OUTPUT := $(BUILD_DIR)/kernel.elf

CFLAGS += \
	-I$(SRC_DIR)/kernel/arch/$(ARCH_DIR)/include \
	-I$(SRC_DIR)/kernel/include \
	-I$(SRC_DIR)/kernel \
	-I$(SRC_DIR)/libk/include

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(ARCH_OBJ) $(BUILD_DIR)/libk/libk.a -o $@ -T $(SDIR)/arch/$(ARCH_DIR)/linker.ld

$(BDIR)/%.c.o: $(SDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BDIR)/%.s.o: $(SDIR)/%.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BDIR)/%.c.d: $(SDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MM $< -MF $@

$(BDIR)/%.s.d: $(SDIR)/%.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MM $< -MF $@

-include $(HEADER_DEPS)