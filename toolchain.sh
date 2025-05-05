#!bin/sh
set -e

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

mkdir -p $HOME/opt/src

cd $HOME/opt/src
wget https://ftp.gnu.org/gnu/binutils/binutils-2.44.tar.gz
tar -xf binutils-2.44.tar.gz
mkdir binutils-build
cd binutils-build
../binutils-2.44/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j8
make install

cd $HOME/opt/src
wget https://ftp.gnu.org/gnu/gdb/gdb-16.3.tar.gz
tar -xf ./gdb-16.3.tar.gz
mkdir gdb-build
cd gdb-build
../gdb-16.3/configure --target=$TARGET --prefix="$PREFIX" --disable-werror
make -j8 all-gdb
make install-gdb

cd $HOME/opt/src
wget https://ftp.gnu.org/gnu/gcc/gcc-15.1.0/gcc-15.1.0.tar.gz
tar -xf ./gcc-15.1.0.tar.gz

which -- $TARGET-as || echo $TARGET-as is not in the PATH

mkdir build-gcc
cd build-gcc
../gcc-15.1.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers --disable-hosted-libstdcxx
make -j8 all-gcc
make -j8 all-target-libgcc
make -j8 all-target-libstdc++-v3
make install-gcc
make install-target-libgcc
make install-target-libstdc++-v3

export PATH=$HOME/opt/cross/bin:$PATH