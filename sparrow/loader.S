%include"boot.inc"
section loader vstart=LOADER_BASE_ADDR
    ;---------------------定义描述符----------------------------
    GDT_BASE:
        dd 0X00000000 ;低32位
        dd 0x00000000 ;高32位
    GDT_CODE_DESC:
        dd 0x0000ffff ; 4k*0xfffff=4GB ，这里为4个f，表示段界限15～0
        dd DESC_CODE_HEIGH4
    GDT_DATA_DESC:
        dd 0x0000ffff
        dd DESC_DATA_HEIGH4
    GDT_VIDEO_DESC:
        dd 0x80000007             ;需要映射到0xb8000上 这里取0x8000作为低八位段基地址，段界限为（0xbffff-0xb8000）/4k = 0x7
        dd DESC_VIDEO_HEIGH4
    
    GDT_SIZE EQU $-GDT_BASE
    GDT_LIMIT EQU GDT_SIZE-1
    times 60 dq 0 ;预留60个描述符号

    SELECTOR_CODE equ (0x0001<<3)+TI_GDT+RPL0
    SELECTOR_DATA equ (0x0002<<3)+TI_GDT+RPL0
    SELECTOR_VIDEO equ (0x0003<<3)+TI_GDT+RPL0

    ;------------------------定义描述符结束-----------------------------
    
    ;此处偏移地址512bit，等于16进制0x200 真实地址为0xb00
    ;定义一个变量用于接收内存大小返回值,内存是一个4字节大小的数据 64位
    total_mem_bytes dd 0

    ;以下是 gdt 的指针，前 2 字节是 界限，后 4 字节是 gdt 起始地址
    GDT_PTR:
            dw GDT_LIMIT
            dd GDT_BASE

    ards_buf times 244 db 0  ;到这里刚好256个字节，用来存储244字节的ard结构体，为了人工对齐
    ards_nr dw 0             ;ards结构体的数量

    loader_start:
;----------------------------------获取内存使用ep820---------------------------------------------------
        xor ebx,ebx              ;参数
        mov edx,0x534d4150       ;参数
        mov di,ards_buf          ;返回到ard_buf中
    .e820_mem_get_loop:
        mov eax,0x0000e820       
        mov cx,20                ;20个字节大小
        int 0x15
        add di,cx                ;朝着ards缓冲区往后面移动20个字节
        inc word [ards_nr]         ;ards_nr + 1
        cmp ebx,0                 ;返回成功ebx=0且cf！=1,说明全部读取完成
        jnz .e820_mem_get_loop

        ;冒泡排序找到内存最大值，即为内存容量
        mov cx,[ards_nr] ;计数
        mov ebx,ards_buf ;取缓冲区首地址
        xor edx,edx       ;用于存储最大值

    .find_max_mem_area:
        mov eax,[ebx]  ;lowaddr
        add eax,[ebx+8];heigh addr
        add ebx,20      ;next ards
        cmp edx,eax     ;比较大小
        jge .next_ards  ;如果当前最大值大于该ards地址
        mov edx,eax     ;否则将edx赋值为最大值
    .next_ards:
        loop .find_max_mem_area
        
        mov [total_mem_bytes],edx ;将内存大小保存到内存中
;---------------------------------------------------ep820 end-----------------------------------------------------

;----------------------------------------------open protect mode--------------------------------------------------
;打开A20
        in al,0x92
        or al,0000_0010B
        out 0x92,al 
;加载gdt
        lgdt [GDT_PTR]
;cr0第0位置1
        mov eax,cr0
        or eax,0x00000001
        mov cr0,eax

        jmp dword SELECTOR_CODE:p_mode_start
;------------------------------------------open protect mode------------------------------------------------------
;VGA打印
[bits 32]
p_mode_start:
    ;初始化寄存器
    mov ax,SELECTOR_DATA
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov esp,LOADER_STACK_TOP
    mov ax,SELECTOR_VIDEO  ;选择显存段
    mov gs,ax

    ;加载内核 
                                                            ; -------------------------   加载kernel  ----------------------
    mov eax, KERNEL_START_SECTOR                        ; kernel.bin所在的扇区号
    mov ebx, KERNEL_BIN_BASE_ADDR                       ; 从磁盘读出后，写入到ebx指定的地址
    mov ecx, 200			                            ; 读入的扇区数
    call rd_disk_m_32

    
;开启分页
    call .setup_page
    sgdt [GDT_PTR] ;将gdt的描述符内容保存到gdt_ptr中
    mov ebx,[GDT_PTR+2] ;取出原来的gdt描述符表基地址
    or DWORD [ebx+8+8+8+4],0xc0000000 ;将视频基础地址映射到原始地址加3GB的虚拟地址上
    add dword [GDT_PTR + 2], 0xc0000000 ;GDT基地值映射到上面
    add esp,0xc0000000
    ;把页目录给cr3
    mov eax,PAGE_DIR_TABLE_POS
    mov cr3,eax
    ;带开cr0的pg
    mov ebx,cr0
    or ebx,0x80000000
    mov cr0,ebx
    ;重新加载段机制
    lgdt [GDT_PTR]

;进入内核吧
    jmp SELECTOR_CODE:enter_kernel



;---------------------------创建页目录 PDE 和页表 PTE-----------------------------------------------------------
.setup_page:
        ;清理出页目录的空间出来，值为0
        mov ecx,4096
        mov esi,0

    .clear_page_dir:
        mov byte [PAGE_DIR_TABLE_POS+esi],0
        inc esi
        loop .clear_page_dir

    ;开始创建PDE
    .create_pde:
        mov eax,PAGE_DIR_TABLE_POS
        add eax,0X1000  ;eax+4096 的地址等于第一个页表的地址，页目录占据 1024*4 Byte大小  4096=1024个4字节
        ;每一位地址可以访问一个字节的单元
        ;此时eax表示第一个1页表地址
        mov ebx,eax;ebx 作为页表基地址

        or eax,PG_P|PG_RW_W|PG_US_U ;赋值属性

        mov [PAGE_DIR_TABLE_POS+0x0],eax ;页目录指向第一个页表

        mov [PAGE_DIR_TABLE_POS+0xc00],eax ;第768页目录指向第一个页表，也就是虚拟内存3G开始后的页表指向第一个页表

        sub eax,0x1000
        mov [PAGE_DIR_TABLE_POS+4092],eax ;最后一个页目录指页目录基地址，后面说是为了动态处理页表？拭目以待

    ;创建页表
        mov ecx,256 ;我们的内核大小在1mb以内 / 每页大小4k = 需要256 个页表
        mov esi ,0 ;地址指针，循环地址初始化使用
        mov edx, PG_US_U | PG_RW_W | PG_P ;初始为0和属性

    .create_ptb:
        mov [ebx+esi*4],edx
        add edx,4096  ;? 原来是每次4kb，256就是1mb，也就是说，第一个页表单独指向 0～1mb的物理地址
        inc esi
        loop .create_ptb

    ;创建内核其他页表
        mov eax,PAGE_DIR_TABLE_POS
        add eax,0x2000 ;第二个页表
        or eax,PG_US_U | PG_RW_W | PG_P ;
        mov ebx,PAGE_DIR_TABLE_POS
        mov ecx,254;769-1022
        mov esi,769
    .create_kernel_pde:
        mov [esi*4+ebx],eax ;指向的2+n个页表
        inc esi
        add eax,0x1000
        loop .create_kernel_pde
        ret 

enter_kernel:
   
   
    call kernel_init
    mov esp, 0xc009f000
    jmp KERNEL_ENTRY_POINT 
;将kernel.bin中的segment拷贝到编译地址
    mov byte [gs:160], "k"
    jmp $
;32位模式下读取磁盘
                                                        ;-------------------------------------------------------------------------------
                                                        ;功能:读取硬盘n个扇区
rd_disk_m_32:	   
                                                        ;-------------------------------------------------------------------------------
				                                        ; eax=LBA扇区号
				                                        ; ebx=将数据写入的内存地址
				                                        ; ecx=读入的扇区数
    mov esi,eax	                                        ;备份eax
    mov di,cx		                                    ;备份cx
                                                        ;读写硬盘:
                                                        ;第1步：选择特定通道的寄存器，设置要读取的扇区数
    mov dx,0x1f2
    mov al,cl
    out dx,al                                           ;读取的扇区数

    mov eax,esi	                                        ;恢复ax

                                                        ;第2步：在特定通道寄存器中放入要读取扇区的地址，将LBA地址存入0x1f3 ~ 0x1f6
                                                        ;LBA地址7~0位写入端口0x1f3
    mov dx,0x1f3                       
    out dx,al                          

                                                        ;LBA地址15~8位写入端口0x1f4
    mov cl,8
    shr eax,cl
    mov dx,0x1f4
    out dx,al

                                                        ;LBA地址23~16位写入端口0x1f5
    shr eax,cl
    mov dx,0x1f5
    out dx,al

    shr eax,cl
    and al,0x0f	                                        ;lba第24~27位
    or al,0xe0	                                        ; 设置7～4位为1110,表示lba模式
    mov dx,0x1f6
    out dx,al

                                                        ;第3步：向0x1f7端口写入读命令，0x20 
    mov dx,0x1f7
    mov al,0x20                        
    out dx,al

                                                        ;第4步：检测硬盘状态
.not_ready:
                                                        ;同一端口，写时表示写入命令字，读时表示读入硬盘状态
    nop
    in al,dx
    and al,0x88	                                        ;第4位为1表示硬盘控制器已准备好数据传输，第7位为1表示硬盘忙
    cmp al,0x08
    jnz .not_ready	                                    ;若未准备好，继续等。

                                                        ;第5步：从0x1f0端口读数据
    mov ax, di                                          ;di当中存储的是要读取的扇区数
    mov dx, 256                                         ;每个扇区512字节，一次读取两个字节，所以一个扇区就要读取256次，与扇区数相乘，就等得到总读取次数
    mul dx                                              ;8位乘法与16位乘法知识查看书p133,注意：16位乘法会改变dx的值！！！！
    mov cx, ax	                                        ; 得到了要读取的总次数，然后将这个数字放入cx中
    mov dx, 0x1f0
.go_on_read:
    in ax,dx
    mov [ebx],ax                                        ;与rd_disk_m_16相比，就是把这两句的bx改成了ebx
    add ebx,2		        
                                                        ; 由于在实模式下偏移地址为16位,所以用bx只会访问到0~FFFFh的偏移。
                                                        ; loader的栈指针为0x900,bx为指向的数据输出缓冲区,且为16位，
                                                        ; 超过0xffff后,bx部分会从0开始,所以当要读取的扇区数过大,待写入的地址超过bx的范围时，
                                                        ; 从硬盘上读出的数据会把0x0000~0xffff的覆盖，
                                                        ; 造成栈被破坏,所以ret返回时,返回地址被破坏了,已经不是之前正确的地址,
                                                        ; 故程序出会错,不知道会跑到哪里去。
                                                        ; 所以改为ebx代替bx指向缓冲区,这样生成的机器码前面会有0x66和0x67来反转。
                                                        ; 0X66用于反转默认的操作数大小! 0X67用于反转默认的寻址方式.
                                                        ; cpu处于16位模式时,会理所当然的认为操作数和寻址都是16位,处于32位模式时,
                                                        ; 也会认为要执行的指令是32位.
                                                        ; 当我们在其中任意模式下用了另外模式的寻址方式或操作数大小(姑且认为16位模式用16位字节操作数，
                                                        ; 32位模式下用32字节的操作数)时,编译器会在指令前帮我们加上0x66或0x67，
                                                        ; 临时改变当前cpu模式到另外的模式下.
                                                        ; 假设当前运行在16位模式,遇到0X66时,操作数大小变为32位.
                                                        ; 假设当前运行在32位模式,遇到0X66时,操作数大小变为16位.
                                                        ; 假设当前运行在16位模式,遇到0X67时,寻址方式变为32位寻址
                                                        ; 假设当前运行在32位模式,遇到0X67时,寻址方式变为16位寻址.
    loop .go_on_read
    ret

;内核初始化函数
kernel_init:
    xor eax, eax                                        ;清空eax
    xor ebx, ebx		                                ;清空ebx, ebx记录程序头表地址
    xor ecx, ecx		                                ;清空ecx, cx记录程序头表中的program header数量
    xor edx, edx		                                ;清空edx, dx 记录program header尺寸

    mov dx, [KERNEL_BIN_BASE_ADDR + 42]	                ; 偏移文件42字节处的属性是e_phentsize,表示program header table中每个program header大小
    mov ebx, [KERNEL_BIN_BASE_ADDR + 28]                ; 偏移文件开始部分28字节的地方是e_phoff,表示program header table的偏移，ebx中是第1 个program header在文件中的偏移量
					                                    ; 其实该值是0x34,不过还是谨慎一点，这里来读取实际值
    add ebx, KERNEL_BIN_BASE_ADDR                       ; 现在ebx中存着第一个program header的内存地址
    mov cx, [KERNEL_BIN_BASE_ADDR + 44]                 ; 偏移文件开始部分44字节的地方是e_phnum,表示有几个program header
.each_segment:
    cmp byte [ebx + 0], PT_NULL		                    ; 若p_type等于 PT_NULL,说明此program header未使用。
    je .PTNULL

                                                        ;为函数memcpy压入参数,参数是从右往左依然压入.函数原型类似于 memcpy(dst,src,size)
    push dword [ebx + 16]		                        ; program header中偏移16字节的地方是p_filesz,压入函数memcpy的第三个参数:size
    mov eax, [ebx + 4]			                        ; 距程序头偏移量为4字节的位置是p_offset，该值是本program header 所表示的段相对于文件的偏移
    add eax, KERNEL_BIN_BASE_ADDR	                    ; 加上kernel.bin被加载到的物理地址,eax为该段的物理地址
    push eax				                            ; 压入函数memcpy的第二个参数:源地址
    push dword [ebx + 8]			                    ; 压入函数memcpy的第一个参数:目的地址,偏移程序头8字节的位置是p_vaddr，这就是目的地址
    call mem_cpy				                        ; 调用mem_cpy完成段复制
    add esp,12				                            ; 清理栈中压入的三个参数
.PTNULL:
   add ebx, edx				                            ; edx为program header大小,即e_phentsize,在此ebx指向下一个program header 
   loop .each_segment
   ret


                                                        ;----------  逐字节拷贝 mem_cpy(dst,src,size) ------------
                                                        ;输入:栈中三个参数(dst,src,size)
                                                        ;输出:无
                                                        ;---------------------------------------------------------
mem_cpy:		      
    cld                                                 ;将FLAG的方向标志位DF清零，rep在执行循环时候si，di就会加1
    push ebp                                            ;这两句指令是在进行栈框架构建
    mov ebp, esp
    push ecx		                                    ; rep指令用到了ecx，但ecx对于外层段的循环还有用，故先入栈备份
    mov edi, [ebp + 8]	                                ; dst，edi与esi作为偏移，没有指定段寄存器的话，默认是ss寄存器进行配合
    mov esi, [ebp + 12]	                                ; src
    mov ecx, [ebp + 16]	                                ; size
    rep movsb		                                    ; 逐字节拷贝

                                                        ;恢复环境
    pop ecx		
    pop ebp
    ret

     