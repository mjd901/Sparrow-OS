
dd if=build/mbr.bin of=/dev/sdd1 bs=512 count=1 conv=notrunc
dd if=build/loader.bin of=/dev/sdd1 bs=512 count=4 seek=2 conv=notrunc
dd if=build/kernel.bin of=/dev/sdd1 bs=512 count=200 seek=9 conv=notrunc