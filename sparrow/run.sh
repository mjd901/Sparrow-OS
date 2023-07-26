nasm -I include/ -o mbr.bin mbr.S #包含头文件添加 -I
nasm -I include/ -o loader.bin loader.S
i686-elf-gcc -c -o kernel/main.o kernel/main.c
i686-elf-ld kernel/main.o -Ttext 0xc0001500 -e main -o kernel/kernel.bin  # -e指定入口函数

dd if=mbr.bin of=/home/mjd/gcc-cross-compiler/bin/hd60.img bs=512 count=1 conv=notrunc
dd if=loader.bin of=/home/mjd/gcc-cross-compiler/bin/hd60.img bs=512 count=4 seek=2 conv=notrunc
dd if=kernel/kernel.bin of=/home/mjd/gcc-cross-compiler/bin/hd60.img bs=512 count=200 seek=9 conv=notrunc
bochs -f /home/mjd/gcc-cross-compiler/bin/bochsrc.disk