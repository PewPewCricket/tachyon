!# bin/bash

qemu-system-i386 --cdrom os.iso -S -s &
gdb -tui ./build/bin/$1 -ex "target remote localhost:1234"