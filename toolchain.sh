#!/bin/bash

set -euo pipefail

CDIR=$1
CROSS_SRC="${CDIR}/cross/src"
HOST="x86_64-pc-linux-gnu"

PREFIX32="${CDIR}/cross/i686"
TARGET32=i686-elf
PREFIX64="${CDIR}/cross/amd64"
TARGET64=x86_64-elf

mkdir -p "$CROSS_SRC"
pushd "$CROSS_SRC" || exit

if [[ "$2" != "offline" ]]; then
  wget https://ftp.gnu.org/gnu/binutils/binutils-2.44.tar.gz
  tar -xf binutils-2.44.tar.gz
  wget https://ftp.gnu.org/gnu/gdb/gdb-16.3.tar.gz
  tar -xf ./gdb-16.3.tar.gz
  wget https://ftp.gnu.org/gnu/gcc/gcc-15.1.0/gcc-15.1.0.tar.gz
  tar -xf ./gcc-15.1.0.tar.gz
fi

# =========================
# 32 bit toolchain
# =========================

mkdir binutils-build-32
cd binutils-build-32 || exit
../binutils-2.44/configure --target=$TARGET32 --prefix="$PREFIX32" --host="$HOST" --disable-nls --disable-werror --enable-languages=c --without-headers --disable-libgomp --disable-libatomic
make -j32
make install
cd .. || exit

mkdir gdb-build-32
cd gdb-build-32 || exit
../gdb-16.3/configure --target=$TARGET32 --prefix="$PREFIX32" --host="$HOST" --disable-nls --disable-werror --enable-languages=c --without-headers --disable-libgomp --disable-libatomic
make -j32 all-gdb
make install-gdb
cd .. || exit

mkdir gcc-build-32
cd gcc-build-32 || exit
../gcc-15.1.0/configure --target=$TARGET32 --prefix="$PREFIX32" --host="$HOST" --disable-nls --disable-werror --enable-languages=c --without-headers --disable-libgomp --disable-libatomic
make -j32 all-gcc
make -j32 all-target-libgcc
make install-gcc
make install-target-libgcc
cd .. || exit

# =========================
# 64 bit toolchain
# =========================

mkdir binutils-build-64
cd binutils-build-64 || exit
../binutils-2.44/configure --target=$TARGET64 --prefix="$PREFIX64" --host="$HOST" --with-pic --disable-nls --disable-werror --enable-languages=c --without-headers --disable-libgomp --disable-libatomic
make -j32
make install
cd .. || exit

mkdir gdb-build-64
cd gdb-build-64 || exit
../gdb-16.3/configure --target=$TARGET64 --prefix="$PREFIX64" --host="$HOST" --with-pic --disable-nls --disable-werror --enable-languages=c --without-headers --disable-libgomp --disable-libatomic
make -j32 all-gdb
make install-gdb
cd .. || exit

mkdir gcc-build-64
pushd gcc-build-64 || exit
../gcc-15.1.0/configure --target=$TARGET64 --prefix="$PREFIX64" --host="$HOST" --with-pic --disable-nls --disable-werror --enable-languages=c --without-headers --disable-libgomp --disable-libatomic
make -j32 all-gcc
make -j32 all-target-libgcc
make install-gcc
make install-target-libgcc
cd .. || exit

popd || exit