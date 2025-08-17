CFLAGS += -m64 -Wl,-m,elf_x86_64
TYPE ?= release

release: CFLAGS += -O2
debug: CFLAGS += -Og -g

SRCFILES := $(shell find -L * -type f | LC_ALL=C sort)
CFILES := $(filter %.c,$(SRCFILES))
ASFILES := $(filter %.S,$(SRCFILES))

OBJ = $(addprefix $(BDIR)/kernel/obj/$(TYPE)/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o))
DEPS = $(addprefix $(BDIR)/kernel/obj/$(TYPE)/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))
ELF = $(BDIR)/kernel/kernel.elf

.PHONY: all release debug $(ELF)

all: release

release:
	$(MAKE) TYPE=release $(ELF)

debug:
	$(MAKE) TYPE=debug $(ELF)

$(ELF): $(OBJ) linker.ld Makefile
	mkdir -p $(dir $@)
	gcc $(CFLAGS) $(OBJ) -o $@ -T linker.ld

$(BDIR)/kernel/obj/$(TYPE)/%.c.o: %.c Makefile
	mkdir -p $(dir $@)
	gcc $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BDIR)/kernel/obj/$(TYPE)/%.S.o: %.S Makefile
	mkdir -p $(dir $@)
	gcc $(CFLAGS) $(CPPFLAGS) -c $< -o $@

-include $(DEPS)