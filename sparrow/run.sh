nasm -I include/ -o build/mbr.bin mbr.S #包含头文件添加 -I
nasm -I include/ -o build/loader.bin loader.S
nasm -f elf -o build/print.o  kernel/print.S 
nasm -f elf -o build/kernel.o  kernel/kernel.S
i686-elf-gcc -I include/lib/kernel -I kernel/ -c -fno-builtin -o build/init.o kernel/init.c
i686-elf-gcc -I include/lib/kernel -I kernel/ -c -fno-builtin -o build/interrupt.o kernel/interrupt.c
i686-elf-gcc -I /home/mjd/os-dev/sparrow-os/sparrow/include/lib/kernel -c -o build/main.o kernel/main.c 
i686-elf-ld -Ttext 0xc0001500 -e main -o build/kernel.bin  build/main.o build/init.o build/interrupt.o build/kernel.o build/print.o # 注意，调用在前，实现在后，否则会出错


dd if=build/mbr.bin of=/home/mjd/gcc-cross-compiler/bin/hd60.img bs=512 count=1 conv=notrunc
dd if=build/loader.bin of=/home/mjd/gcc-cross-compiler/bin/hd60.img bs=512 count=4 seek=2 conv=notrunc
dd if=build/kernel.bin of=/home/mjd/gcc-cross-compiler/bin/hd60.img bs=512 count=200 seek=9 conv=notrunc
bochs -f /home/mjd/gcc-cross-compiler/bin/bochsrc.disk