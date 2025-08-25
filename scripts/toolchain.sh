#!/bin/bash
set -eo pipefail

CDIR=`pwd`
CROSS_SRC="${CDIR}/cross/src"
HOST="x86_64-pc-linux-gnu"

PREFIX64="${CDIR}/cross"
TARGET64=x86_64-elf

BINUTILS_VERSION="2.44"
GDB_VERSION="16.3"
GCC_VERSION="15.1.0"

if [ -z "$1" ]; then set -- online; fi

mkdir -p "$CROSS_SRC"
pushd "$CROSS_SRC" || exit

if [[ "$1" != "offline" ]]; then
  wget https://ftpmirror.gnu.org/gnu/binutils/binutils-"$BINUTILS_VERSION".tar.gz
  wget https://ftpmirror.gnu.org/gnu/gdb/gdb-"$GDB_VERSION".tar.gz
  wget https://ftpmirror.gnu.org/gnu/gcc/gcc-"$GCC_VERSION"/gcc-"$GCC_VERSION".tar.gz
fi

if [ ! -d ./binutils-"$BINUTILS_VERSION" ]; then tar -xf ./binutils-"$BINUTILS_VERSION".tar.gz; fi
if [ ! -d ./gdb-"$GDB_VERSION" ]; then tar -xf ./gdb-"$GDB_VERSION".tar.gz; fi
if [ ! -d ./gcc-"$GCC_VERSION" ]; then tar -xf ./gcc-"$GCC_VERSION".tar.gz; fi

mkdir binutils-build ||:
cd binutils-build || exit
../binutils-"$BINUTILS_VERSION"/configure --target=$TARGET64 --prefix="$PREFIX64" --host="$HOST" --with-pic --disable-nls --disable-werror --enable-languages=c --without-headers --disable-libgomp --disable-libatomic
make -j32
make install
cd .. || exit

mkdir gdb-build ||:
cd gdb-build || exit
../gdb-"$GDB_VERSION"/configure --target=$TARGET64 --prefix="$PREFIX64" --host="$HOST" --with-pic --disable-nls --disable-werror --enable-languages=c --without-headers --disable-libgomp --disable-libatomic
make -j32 all-gdb
make install-gdb
cd .. || exit

mkdir gcc-build ||:
pushd gcc-build || exit
../gcc-"$GCC_VERSION"/configure --target=$TARGET64 --prefix="$PREFIX64" --host="$HOST" --with-pic --disable-nls --disable-werror --enable-languages=c --without-headers --disable-libgomp --disable-libatomic
make -j32 all-gcc
make -j32 all-target-libgcc
make install-gcc
make install-target-libgcc
cd .. || exit

popd || exit
