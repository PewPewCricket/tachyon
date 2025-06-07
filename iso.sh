rm -r ./build/isodir
rm ./build/tachyon.iso
mkdir -p ./build/isodir/boot/grub
cp ./build/kernel.elf ./build/isodir/boot/kernel.elf
cp loader/grub2/grub/grub.cfg ./build/isodir/boot/grub/grub.cfg
grub-mkrescue -o ./build/tachyon.iso ./build/isodir