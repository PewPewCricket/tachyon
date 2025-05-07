export

PDIR := $(SRCDIR)/libc

SRCFILES := $(shell cd $(PDIR) && find -L * -type f | grep -v "arch" | LC_ALL=C sort)
CFILES := $(filter %.c,$(SRCFILES))
OBJ := $(CFILES:.c=.c.o)
HEADER_DEPS := $(CFILES:.c=.c.d)
OBJ_LIBK := $(CFILES:.c=.c.libk.o)
HEADER_DEPS_LIBK := $(CFILES:.c=.c.libk.d)

CFLAGS += 
CPPFLAGS += 
LIBK_CFLAGS += $(CFLAGS)
LIBK_CPPFLAGS += $(CPPFLAGS) -D__is_libk

all: libc.a libk.a

# This needs a working kernel and system to work >->
libc.a: $(OBJ)
	@echo "  AR      " $@
	@ar rcs $@ $(OBJ)
	@ranlib $@

libk.a: $(OBJ_LIBK)
	@echo "  AR      " $@
	@ar rcs $@ $(OBJ_LIBK)
	@ranlib $@

%.c.o: %.c
	@echo "  CC      " $<
	@$(CC) $(CFLAGS) -c $< -o $@

%.c.libk.o: %.c
	@echo "  CC      " $<
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: install
install: all
	@echo "Installing libc.a into $(DESTDIR)/lib/"
	@mkdir -p $(DESTDIR)/lib/
	@cp libc.a $(DESTDIR)/lib/

.PHONY: clean
clean:
	@echo "  RM      " libc.a libk.a $(OBJ) $(HEADER_DEPS)
	-@rm -rf $(OBJ) $(HEADER_DEPS) $(OBJ_LIBK) $(HEADER_DEPS_LIBK) libc.a libk.a

-include $(HEADER_DEPS)
-include $(HEADER_DEPS_LIBK)