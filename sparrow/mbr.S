;主引导程序
%include "boot.inc"
SECTION MBR vstart=0x7c00 ;mbr在内存中的物理地址开始
    ;初始化状态寄存器,此时bios执行程序jmp 0：0x7c00 所以cs=0
    mov ax,cs 
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov fs,ax
    mov sp,0x7c00 ;程序依赖栈，所以需要设置栈顶，入栈时指针-1。目前 Ox7c00 以下暂时是安全的区域，就把它当作找来用。
    mov ax,0xb800
    mov gs,ax

    ;;;;;;;;;;;;;;;;;;;;;显示字符;;;;;;;;;;;;;;;;;;;;;;;;;
    ;VGA清屏幕
    mov ah,0x06 ;06号功能清屏幕
    mov al,0x00 ;0表示清除所有行
    mov bx,0x700
    mov cx,0      ;坐标左上角(0,0)
    mov dx,0x184f ;坐标右下角(80,25)
    int 0x10      ;调用10号中断

    mov byte [gs:0x00], "1"
    mov byte [gs:0x01], 0xa4

    mov byte [gs:0x02], 'M'
    mov byte [gs:0x03], 0xa4
    mov byte [gs:0x04], 'B'
    mov byte [gs:0x05], 0xa4
    mov byte [gs:0x06], 'R'
    mov byte [gs:0x07], 0xa4
    ;;;;;;;;;;;;;;;;;;;;;;显示字符结束;;;;;;;;;;;;;;;;;;;;

    mov eax,LOADER_START_SECTOR    ;地二个扇区                           
    mov bx,LOADER_BASE_ADDR        ;;加载loader到1mb空间的0x900处，也是
    mov cx,4                    ;读入的扇区数量
    call rd_disk_m_16           ;实模式下读取磁盘
    jmp LOADER_BASE_ADDR+0x300
rd_disk_m_16:
        ;参数1 eax,参数2 bx， 参数3 cx
        mov esi,eax  ;eax的low8位会被用到，所以这里备份
        mov di,cx    ;cx后面会用到备份

;read the disk 
;first step:set the number of sector
;1.先选择通道，往该通道的 sector count 寄存器中写入待操作的扇区数。参考表3--17
        mov dx,0x1f2
        mov al,cl 
        out dx,al   ;在0x1f2端口存放cl个扇区
        mov eax,esi ;还原eax

;2. 将LBA地址存放到0x1f3~0x1f6
        ;LBA 7~0位
        mov dx,0x1f3
        out dx,al 
        ;LBA 15~8 位
        mov cl,8
        shr eax,cl ;右移动8位，此时底8位刚好对应15-8
        mov dx,0x1f4
        out dx,al  ;

        ;LBA 23~16位
        shr eax,cl
        mov dx,0x1f5
        out dx,al

        ;device 寄存器
        shr eax,cl 
        and al,0x0f ;lba 第 24-27 位
        or al,0xe0   ;设置 7-4 位为 1110 ，表示 lba 模式
        mov dx,0x1f6
        out dx,al 

;第三步 ：向0x1f7端口写入读命令，0x20
        mov dx,0x1f7
        mov al,0x20
        out dx,al 
        
;第4步：检查硬件状态
    .not_ready:
        nop ;让cpu别来打扰读取硬盘的操作
        in al,dx
        and al,0x88 ;第 4 位为 1 表示硬盘控制器已准备好数据传输
                    ;第 7 位为 1 表示硬盘忙
        cmp al,0x08 ;如果读取万数据后status寄存器第4位为1,al-0x08=0
        jnz .not_ready ;if al-0x08!=0:继续执行该段

;第5步：从0x1f0端口读取数据
        mov ax,di   ;2个扇区
        mov dx,256  ;一个扇区需要读取256次
        mul dx      ;ax=ax*dx 如果是32位结果，那么高位方到dx中，ax低16位
        mov cx,ax   ;cx存储着要读取的次数，每次读取一个子，2个字节，一个扇区512字节
                    ;所以需要cx次
        mov dx,0x1f0 ;从0x1f端口读数据
    .go_on_read
        in ax,dx ;读入的数据放在ax中
        mov [bx] ,ax
        add bx,2 ;每次增加1个字的空间
        loop .go_on_read ;当cx=0时停止循环
        ret ;函数结束 恢复cs:ip

    



    jmp $ ;让程序停止到此处
    times 510 - ($-$$) db 0
    db 0x55,0xaa