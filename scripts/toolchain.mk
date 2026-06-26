TOOL_TARGET := $(toolchain-y)-elf
HOST_ENV := CFLAGS="" LDFLAGS=""

GCC_VERSION := 15.3.0
BINUTILS_VERSION := 2.46.1
GDB_VERSION := 17.2

# GCC
$(TOOL_DIR)/bin/$(TOOL_TARGET)-gcc: $(TOOL_SRC)/gcc-$(GCC_VERSION) $(TOOL_DIR)/bin/$(TOOL_TARGET)-ld
	-mkdir -p $(TOOL_BUILD)/gcc-$(GCC_VERSION)
	cd $(TOOL_BUILD)/gcc-$(GCC_VERSION) && \
	$(HOST_ENV) $(TOOL_SRC)/gcc-$(GCC_VERSION)/configure \
		--target=$(TOOL_TARGET) \
		--prefix="$(TOOL_DIR)" \
		--disable-nls \
		--enable-languages=c,c++ \
		--without-headers \
		--enable-initfini-array \
		--disable-hosted-libstdcxx && \
	make -j$$(nproc) all-gcc && \
	make -j$$(nproc) all-target-libgcc && \
	make -j$$(nproc) all-target-libstdc++-v3 && \
	make install-gcc && \
    make install-target-libgcc && \
    make install-target-libstdc++-v3


$(TOOL_SRC)/gcc-$(GCC_VERSION): $(TOOL_SRC)/gcc-$(GCC_VERSION).tar.xz
	tar xf $(TOOL_SRC)/gcc-$(GCC_VERSION).tar.xz -C $(TOOL_SRC)
	cd $(TOOL_SRC)/gcc-$(GCC_VERSION) && \
	./contrib/download_prerequisites && \
	patch -p1 < $(TOP_DIR)/patch/$(toolchain-y)-gcc.patch

$(TOOL_SRC)/gcc-$(GCC_VERSION).tar.xz:
	wget -P $(TOOL_SRC) https://mirrorservice.org/sites/sourceware.org/pub/gcc/releases/gcc-15.3.0/gcc-15.3.0.tar.xz

# GDB
$(TOOL_DIR)/bin/$(TOOL_TARGET)-gdb: $(TOOL_SRC)/gdb-$(GDB_VERSION)
	-mkdir -p $(TOOL_BUILD)/gdb-$(GDB_VERSION)
	cd $(TOOL_BUILD)/gdb-$(GDB_VERSION) && \
	$(HOST_ENV) $(TOOL_SRC)/gdb-$(GDB_VERSION)/configure \
    	--target=$(TOOL_TARGET) \
    	--prefix="$(TOOL_DIR)" \
    	--disable-werror && \
	make -j$$(nproc) && \
	make install

$(TOOL_SRC)/gdb-$(GDB_VERSION): $(TOOL_SRC)/gdb-$(GDB_VERSION).tar.xz
	tar xf $(TOOL_SRC)/gdb-$(GDB_VERSION).tar.xz -C $(TOOL_SRC)

$(TOOL_SRC)/gdb-$(GDB_VERSION).tar.xz:
	wget -P $(TOOL_SRC) https://ftp.gnu.org/gnu/gdb/gdb-17.2.tar.xz

# Binutils
$(TOOL_DIR)/bin/$(TOOL_TARGET)-ld: $(TOOL_SRC)/binutils-$(BINUTILS_VERSION)
	-mkdir -p $(TOOL_BUILD)/binutils-$(BINUTILS_VERSION)
	cd $(TOOL_BUILD)/binutils-$(BINUTILS_VERSION) && \
	$(HOST_ENV) $(TOOL_SRC)/binutils-$(BINUTILS_VERSION)/configure \
    		--target=$(TOOL_TARGET) \
    		--prefix="$(TOOL_DIR)" \
    		--with-sysroot \
    		--disable-nls \
    		--disable-werror \
    		--enable-default-execstack=no && \
	make -j$$(nproc) && \
	make install

$(TOOL_SRC)/binutils-$(BINUTILS_VERSION): $(TOOL_SRC)/binutils-$(BINUTILS_VERSION).tar.xz
	tar xf $(TOOL_SRC)/binutils-$(BINUTILS_VERSION).tar.xz -C $(TOOL_SRC)

$(TOOL_SRC)/binutils-$(BINUTILS_VERSION).tar.xz:
	wget -P $(TOOL_SRC) https://ftp.gnu.org/gnu/binutils/binutils-2.46.1.tar.xz