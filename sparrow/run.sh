nasm -I include/ -o mbr.bin mbr.S #包含头文件添加 -I
nasm -I include/ -o loader.bin loader.S
nasm -f elf -o print.o  kernel/print.S 
i686-elf-gcc -I /home/mjd/os-dev/sparrow-os/sparrow/include/lib/kernel -c -o kernel/main.o kernel/main.c 
i686-elf-ld -Ttext 0xc0001500 -e main -o kernel/kernel.bin  kernel/main.o print.o  # 注意，调用在前，实现在后，否则会出错
dd if=mbr.bin of=/home/mjd/gcc-cross-compiler/bin/hd60.img bs=512 count=1 conv=notrunc
dd if=loader.bin of=/home/mjd/gcc-cross-compiler/bin/hd60.img bs=512 count=4 seek=2 conv=notrunc
dd if=kernel/kernel.bin of=/home/mjd/gcc-cross-compiler/bin/hd60.img bs=512 count=200 seek=9 conv=notrunc
bochs -f /home/mjd/gcc-cross-compiler/bin/bochsrc.disk