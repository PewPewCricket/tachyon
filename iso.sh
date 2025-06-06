rm -r isodir
rm tachyon.iso
mkdir -p isodir/boot/grub
cp ./build/kernel.elf isodir/boot/kernel.elf
cp loader/grub2/grub/grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o tachyon.iso isodir