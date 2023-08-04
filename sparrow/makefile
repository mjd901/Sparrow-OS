BUILD_DIR = ./build
ENTRY_POINT = 0XC0001500
AS = nasm
CC = i686-elf-gcc
LD = i686-elf-ld 
LIB = -I include/lib/kernel -I kernel/ -I include/lib/user -I include/lib
ASFLAGS = -f elf 
BOOTASFLAGS =  -I include/
HD_PATH = /home/mjd/gcc-cross-compiler/bin/hd60.img
LDFLAGS = -Ttext $(ENTRY_POINT)  -e  main -Map $(BUILD_DIR)/kernel.map  -m elf_i386
#map文件包含一些地址的映射文件等

# -Wall ：尽可能的产生警告 -fno-builtin:当函数同名的时候不要使用c语言内部的函数  -W:显示警告 -Wstrict-prototypes:要求函数声明必须要有参数类型，否则警告 -Wmissing-prototypes：\
必需要有函数声明，否则警告
CFLAGS = -Wall $(LIB) -m32 -c -fno-builtin -W -Wstrict-prototypes -Wmissing-prototypes 
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/init.o $(BUILD_DIR)/interrupt.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/print.o \
$(BUILD_DIR)/string.o $(BUILD_DIR)/debug.o 

#编译开始
$(BUILD_DIR)/main.o: kernel/main.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/init.o: kernel/init.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/interrupt.o: kernel/interrupt.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/string.o: include/lib/string.c 
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/debug.o: kernel/debug.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/kernel.o: kernel/kernel.S
	$(AS) $(ASFLAGS)  $< -o $@

$(BUILD_DIR)/print.o: kernel/print.S
	$(AS) $(ASFLAGS)  $< -o $@

#链接所有的obj
$(BUILD_DIR)/kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/mbr.bin: mbr.S
	$(AS) $(BOOTASFLAGS)  $< -o $@

$(BUILD_DIR)/loader.bin: loader.S
	$(AS) $(BOOTASFLAGS)  $< -o $@


#

.PHONY:clean mk_dir hd clean all build

clean:
	rm ./build/*.*

mk_dir:
	if [[! -d $(BUILD_DIR)]];then mkdir $(BUILD_DIR);fi

hd:
	dd if=$(BUILD_DIR)/mbr.bin of=$(HD_PATH) bs=512 count=1 conv=notrunc
	dd if=$(BUILD_DIR)/loader.bin of=$(HD_PATH) bs=512 count=4 seek=2 conv=notrunc
	dd if=$(BUILD_DIR)/kernel.bin of=$(HD_PATH) bs=512 count=200 seek=9 conv=notrunc

build:$(BUILD_DIR)/mbr.bin $(BUILD_DIR)/loader.bin $(BUILD_DIR)/kernel.bin

all:  build hd
	bochs -f /home/mjd/gcc-cross-compiler/bin/bochsrc.disk