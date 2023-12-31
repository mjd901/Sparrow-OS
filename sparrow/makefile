BUILD_DIR = build
ENTRY_POINT = 0XC0001500
AS = nasm
CC = i686-elf-gcc
LD = i686-elf-ld 
LIB= -I lib/ -I lib/kernel/ -I lib/user/ -I kernel/ -I device/ -I thread/ -I userprog/ -I fs/ -I shell/
ASFLAGS = -f elf 
BOOTASFLAGS =  -I boot/include/
HD_PATH = /home/mjd/gcc-cross-compiler/bin/hd60.img
LDFLAGS= -Ttext $(ENTRY_POINT) -e main -Map $(BUILD_DIR)/kernel.map -m elf_i386
#map文件包含一些地址的映射文件等

# -Wall ：尽可能的产生警告 -fno-builtin:当函数同名的时候不要使用c语言内部的函数  -W:显示警告 -Wstrict-prototypes:要求函数声明必须要有参数类型，否则警告 -Wmissing-prototypes：\
必需要有函数声明，否则警告
CFLAGS = -Wall $(LIB) -m32 -c -fno-builtin -W -Wstrict-prototypes -Wmissing-prototypes 
#-Map,生成map文件，就是通过编译器编译之后，生成的程序、数据及IO空间信息的一种映射文件
#里面包含函数大小，入口地址等一些重要信息

OBJS=$(BUILD_DIR)/main.o $(BUILD_DIR)/init.o \
	$(BUILD_DIR)/interrupt.o $(BUILD_DIR)/timer.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/assert.o\
	$(BUILD_DIR)/print.o $(BUILD_DIR)/debug.o   $(BUILD_DIR)/string.o $(BUILD_DIR)/bitmap.o \
	$(BUILD_DIR)/memory.o $(BUILD_DIR)/thread.o	$(BUILD_DIR)/list.o	$(BUILD_DIR)/switch.o \
	$(BUILD_DIR)/sync.o	$(BUILD_DIR)/console.o $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/ioqueue.o \
	$(BUILD_DIR)/tss.o	$(BUILD_DIR)/process.o $(BUILD_DIR)/syscall.o $(BUILD_DIR)/syscall-init.o\
	$(BUILD_DIR)/stdio-kernel.o $(BUILD_DIR)/stdio.o $(BUILD_DIR)/file.o $(BUILD_DIR)/wait_exit.o  $(BUILD_DIR)/fs.o \
	$(BUILD_DIR)/ide.o $(BUILD_DIR)/dir.o  $(BUILD_DIR)/inode.o  $(BUILD_DIR)/fork.o $(BUILD_DIR)/shell.o\
	$(BUILD_DIR)/buildin_cmd.o 	$(BUILD_DIR)/exec.o  	$(BUILD_DIR)/pipe.o 
#顺序最好是调用在前，实现在后

######################编译两个启动文件的代码#####################################
boot:$(BUILD_DIR)/mbr.bin $(BUILD_DIR)/loader.bin 
	

$(BUILD_DIR)/mbr.bin: boot/mbr.S
	$(AS) $(BOOTASFLAGS)  $< -o $@

$(BUILD_DIR)/loader.bin: boot/loader.S
	$(AS) $(BOOTASFLAGS)  $< -o $@

######################编译C内核代码###################################################
$(BUILD_DIR)/main.o:kernel/main.c
	$(CC) $(CFLAGS) -o $@ $<	
# $@表示规则中目标文件名的集合这里就是$(BUILD_DIR)/main.o  $<表示规则中依赖文件的第一个，这里就是kernle/main.c 

$(BUILD_DIR)/init.o:kernel/init.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/exec.o:userprog/exec.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/interrupt.o:kernel/interrupt.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/timer.o:device/timer.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/debug.o:kernel/debug.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/string.o:lib/string.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/bitmap.o:lib/kernel/bitmap.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/memory.o:kernel/memory.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/thread.o:thread/thread.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/list.o:lib/kernel/list.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/sync.o:thread/sync.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/console.o:device/console.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/keyboard.o:device/keyboard.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/ioqueue.o:device/ioqueue.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/tss.o:userprog/tss.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/process.o:userprog/process.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/syscall.o:lib/user/syscall.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/assert.o:lib/user/assert.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/syscall-init.o:userprog/syscall-init.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/ide.o:device/ide.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/fs.o:fs/fs.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/dir.o:fs/dir.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/file.o:fs/file.c 
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/inode.o:fs/inode.c
	$(CC) $(CFLAGS) -o $@ $<	

$(BUILD_DIR)/fork.o:userprog/fork.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/shell.o:shell/shell.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/stdio-kernel.o:lib/kernel/stdio-kernel.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/stdio.o:lib/stdio.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/buildin_cmd.o:shell/buildin_cmd.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/wait_exit.o:userprog/wait_exit.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/pipe.o:shell/pipe.c
	$(CC) $(CFLAGS) -o $@ $<
###################编译汇编内核代码#####################################################
$(BUILD_DIR)/kernel.o:kernel/kernel.S 
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/print.o:lib/kernel/print.S
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/switch.o:thread/switch.S
	$(AS) $(ASFLAGS) -o $@ $<
##################链接所有内核目标文件##################################################
$(BUILD_DIR)/kernel.bin:$(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
# $^表示规则中所有依赖文件的集合，如果有重复，会自动去重

.PHONY:mk_dir hd clean build all boot	#定义了6个伪目标
mk_dir:
	if [ ! -d $(BUILD_DIR) ];then mkdir $(BUILD_DIR);fi 
#判断build文件夹是否存在，如果不存在，则创建

hd:
	dd if=$(BUILD_DIR)/mbr.bin of=$(HD_PATH) bs=512 count=1 conv=notrunc
	dd if=$(BUILD_DIR)/loader.bin of=$(HD_PATH) bs=512 count=4 seek=2 conv=notrunc
	dd if=$(BUILD_DIR)/kernel.bin of=$(HD_PATH) bs=512 count=200 seek=9 conv=notrunc
	
clean:
	@cd $(BUILD_DIR) && rm -f ./* && echo "remove ./build all done"
#-f, --force忽略不存在的文件，从不给出提示，执行make clean就会删除build下所有文件

build:$(BUILD_DIR)/kernel.bin
	
#执行build需要依赖kernel.bin，但是一开始没有，就会递归执行之前写好的语句编译kernel.bin

all: mk_dir  clean boot build hd
	bochs -f /home/mjd/gcc-cross-compiler/bin/bochsrc.disk
#make all 就是依次执行mk_dir build hd