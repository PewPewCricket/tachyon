export

SDIR := $(SRC_DIR)/kernel
BDIR := $(BUILD_DIR)/kernel

SRC_FILES := $(shell cd $(SDIR) && find -L * -type f | grep -v "arch" | LC_ALL=C sort)
CFILES := $(filter %.c,$(SRC_FILES))

OBJS := $(addprefix $(BDIR)/, $(CFILES:.c=.c.o))
HEADER_DEPS := $(addprefix $(BDIR)/, $(CFILES:.c=.c.d))
ARCH_OBJ := $(BDIR)/arch/$(ARCH_DIR)/$(ARCH).a
OUTPUT := $(BUILD_DIR)/kernel.img

CFLAGS += \
	-I$(SRC_DIR)/kernel/arch/$(ARCH_DIR)/include \
	-I$(SRC_DIR)/kernel/include \
	-I$(SRC_DIR)/kernel \
	-I$(SRC_DIR)/libk/include

all: $(OUTPUT)

$(OUTPUT): $(OBJ) $(ARCH_OBJ)
	$(LD) $(LDFLAGS) $(OBJS) $(ARCH_OBJ) $(BUILD_DIR)/libk/libk.a -o $@ -T $(SDIR)/arch/$(ARCH_DIR)/linker.ld

$(ARCH_OBJ): arch/$(ARCH_DIR)/Makefile
	$(MAKE) -C $(SDIR)/arch/$(ARCH_DIR)

$(BDIR)/%.c.o: $(SDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BDIR)/%.c.d: $(SDIR)/%.c $(OBJS)
	$(CC) $(CFLAGS) -MM $< -MF $@

-include $(HEADER_DEPS)