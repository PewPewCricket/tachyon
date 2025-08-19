#!/bin/bash

CDIR=$1
PREFIX="${CDIR}/cross/i686"
TARGET=i686-elf
CROSS_SRC="${CDIR}/cross/i686/src"
HOST="x86_64-pc-linux-gnu"
export PATH="$PREFIX/bin:$PATH"

mkdir -p "$CROSS_SRC"
pushd "$CROSS_SRC"

wget https://ftp.gnu.org/gnu/binutils/binutils-2.44.tar.gz
tar -xf binutils-2.44.tar.gz

mkdir binutils-build
pushd binutils-build
../binutils-2.44/configure --target=$TARGET --prefix="$PREFIX" --host="$HOST" --with-sysroot --disable-nls --disable-werror --disable-nls --enable-languages=c,c++ --without-headers --disable-hosted-libstdcxx --disable-libssp --disable-libgomp --disable-libatomic --disable-libsanitizer
make -j32
make install
popd

wget https://ftp.gnu.org/gnu/gdb/gdb-16.3.tar.gz
tar -xf ./gdb-16.3.tar.gz

mkdir gdb-build
pushd gdb-build
../gdb-16.3/configure --target=$TARGET --prefix="$PREFIX" --host="$HOST" --disable-werror --disable-nls --enable-languages=c,c++ --without-headers --disable-hosted-libstdcxx --disable-libssp --disable-libgomp --disable-libatomic --disable-libsanitizer
make -j32 all-gdb
make install-gdb
popd

wget https://ftp.gnu.org/gnu/gcc/gcc-15.1.0/gcc-15.1.0.tar.gz
tar -xf ./gcc-15.1.0.tar.gz

mkdir build-gcc
pushd build-gcc
../gcc-15.1.0/configure --target=$TARGET --prefix="$PREFIX" --host="$HOST" --disable-nls --enable-languages=c,c++ --without-headers --disable-nls --enable-languages=c,c++ --without-headers --disable-hosted-libstdcxx --disable-libssp --disable-libgomp --disable-libatomic --disable-libsanitizer
make -j32 all-gcc
make -j32 all-target-libgcc
make -j32 all-target-libstdc++-v3
make install-gcc
make install-target-libgcc
make install-target-libstdc++-v3
popd


popd