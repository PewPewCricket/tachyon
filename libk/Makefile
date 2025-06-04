SDIR := $(SRC_DIR)/libk
BDIR := $(BUILD_DIR)/libk

SRC_FILES := $(shell cd $(SDIR) && find -L * -type f | LC_ALL=C sort)
CFILES := $(filter %.c,$(SRC_FILES))

OBJS := $(addprefix $(BDIR)/, $(CFILES:.c=.c.o))
HEADER_DEPS := $(addprefix $(BDIR)/, $(CFILES:.c=.c.d))
OUTPUT := $(BDIR)/libk.a

CFLAGS += \
	-I$(SRC_DIR)/kernel/include \
	-I$(SRC_DIR)/kernel \
	-I$(SRC_DIR)/libk/include

$(OUTPUT): $(OBJS) $(HEADER_DEPS)
	$(AR) rcs $@ $(OBJS)

$(BDIR)/%.c.o: $(SDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BDIR)/%.c.d: $(SDIR)/%.c $(OBJS)
	$(CC) $(CFLAGS) -MM $< -MF $@

-include $(HEADER_DEPS)