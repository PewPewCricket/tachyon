obj-y :=
subdir-y:=

include $(BUILD_DIR)/autoconf.mk
include Kbuild

REL_DIR := $(patsubst $(TOPDIR)/%,%,$(CURDIR))
OBJS := $(addprefix $(BUILD_DIR)/$(patsubst $(TOPDIR)/%,%,$(CURDIR))/, $(obj-y))
SUBDIR_OBJS := $(addprefix $(BUILD_DIR)/$(REL_DIR)/,$(addsuffix /built-in.a, $(subdir-y)))

all: $(BUILD_DIR)/$(REL_DIR)/built-in.a

$(BUILD_DIR)/$(REL_DIR)/built-in.a: $(SUBDIR_OBJS) $(OBJS)
	cp $(SUBDIR_OBJS) $@

$(BUILD_DIR)/$(REL_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(REL_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(SUBDIR_OBJS):
	$(MAKE) -C $(patsubst $(BUILD_DIR)/$(REL_DIR)/%,%,$(dir $@))