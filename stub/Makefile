CFLAGS += -m32 -Wl,-m,elf_i386
TYPE ?= release

release: CFLAGS += -O2
debug: CFLAGS += -Og -g

SRCFILES := $(shell find -L * -type f | LC_ALL=C sort)
CFILES := $(filter %.c,$(SRCFILES))
ASFILES := $(filter %.S,$(SRCFILES))

OBJ = $(addprefix $(BDIR)/stub/obj/$(TYPE)/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o))
DEPS = $(addprefix $(BDIR)/stub/obj/$(TYPE)/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))
ELF = $(BDIR)/stub/stub.elf

.PHONY: all release debug $(ELF)

all: release

release:
	$(MAKE) TYPE=release $(ELF)

debug:
	$(MAKE) TYPE=debug $(ELF)

$(ELF): $(OBJ) linker.ld Makefile
	mkdir -p $(dir $@)
	gcc $(CFLAGS) $(OBJ) -o $@ -T linker.ld

$(BDIR)/stub/obj/$(TYPE)/%.c.o: %.c Makefile
	mkdir -p $(dir $@)
	gcc $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BDIR)/stub/obj/$(TYPE)/%.S.o: %.S Makefile
	mkdir -p $(dir $@)
	gcc $(CFLAGS) $(CPPFLAGS) -c $< -o $@

-include $(DEPS)