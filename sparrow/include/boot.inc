;---------------------------loader----------------------------
LOADER_START_SECTOR equ 0x2 ;起始扇区
LOADER_BASE_ADDR equ 0x900 ;将loader代码放到内存0x900处
LOADER_STACK_TOP equ 0x900 ;栈顶
;---------------------------loader end------------------------

;--------------------------GDT 描述符-------------------------
DESC_G_4K equ 1_000_0000_0000_0000_0000_0000b ; 粒度4KB
DESC_D_32 equ  1_00_0000_0000_0000_0000_0000b;   有效地址（段内偏移地址）及操作数的大小
DESC_L_32 equ    00_0000_0000_0000_0000_0000b ;L 为 1 表示 64 位代码段，否则表示 32位代码段
DESC_AVL equ      0_0000_0000_0000_0000_0000b
DESC_LIMIT_CODE2 equ 1111_0000_0000_0000_0000b ;代码段地址
DESC_LIMIT_DATA2 equ DESC_LIMIT_CODE2           ;数据段地址
DESC_LIMIT_VIDEO2 equ 0000_0000_0000_0000_0000b ;显存界限地址
DESC_P equ 1_000_0000_0000_0000b                ;段是否在内存中，不常用的内存会被移除
DESC_DPL_R0 equ 000_0000_0000_0000b                 ;R0
DESC_DPL_R1 equ 010_0000_0000_0000b                 ;R1
DESC_DPL_R2 equ 100_0000_0000_0000b                 ;R3
DESC_DPL_R3 equ 110_0000_0000_0000b                 ;R4
DESC_S   equ 1_0000_0000_0000b                      ;非系统段
DESC_TYPE_CODE equ 1000_0000_0000b                   ;代码段
; x=l,c=O,r=O,a=O 代码段是可执行的，非一致性，不可读，巳访问位 a 清0。
DESC_TYPE_DATA equ 0010_0000_0000b                   ;数据段
; x=O,e=O,w=l,a=O 数据段是不可执行的，向上扩展的，可写，己访问位 a 清0。

DESC_CODE_HEIGH4 EQU (0X00<<24)+DESC_G_4K+DESC_D_32+DESC_L_32+DESC_AVL+DESC_LIMIT_CODE2+DESC_P+DESC_DPL_R0+DESC_S+DESC_TYPE_CODE+0X00
DESC_DATA_HEIGH4 EQU (0X00<<24)+DESC_G_4K+DESC_D_32+DESC_L_32+DESC_AVL+DESC_LIMIT_DATA2+DESC_P+DESC_DPL_R0+DESC_S+DESC_TYPE_DATA+0X00 
DESC_VIDEO_HEIGH4 EQU (0X00<<24)+DESC_G_4K+DESC_D_32+DESC_L_32+DESC_AVL+DESC_LIMIT_VIDEO2+DESC_P+DESC_DPL_R0+DESC_S+DESC_TYPE_DATA+0X0b;0XB8000;段基址

;----------------------------------------GDT 描述符结束----------------------------------------

;----------------------------------------段选择子---------------------------------------------
RPL0 EQU 00B
RPL1 EQU 01B
RPL2 EQU 10B
RPL3 EQU 11B

TI_GDT EQU 000B
TI_LDT EQU 100B

;---------------------------------------页表相关-----------------------------------------------

PAGE_DIR_TABLE_POS equ 0x100000  ;将页表目录设置到1mb开始的位置
PG_P EQU 1B    ;存在在内存中
PG_RW_R equ 00b ;可读
PG_RW_W equ 10b ;可以写
PG_US_S equ 000b ;若为 0 ，表示处于 Supervisor 级 ，rpl=3的不能访问
PG_US_U  equ 100b ;若为 1 时，表示处于 User 级，任意级别（ 0 、 l 、 2 、3 ）特权的程序都可以访问该页

;----------------------------------从硬盘加载扇区和加载segment到内存地址----------------------------------
KERNEL_BIN_BASE_ADDR equ 0x70000                    ;定义内核在内存中的缓冲区，也就是将编译好的内核文件暂时存储在内存中的位置
KERNEL_START_SECTOR equ 0x9                         ;定义内核在磁盘的起始扇区
KERNEL_ENTRY_POINT equ 0xc0001500                   ;定义内核可执行代码的入口地址

                                                    ;-------------  程序段的 type 定义   --------------
PT_NULL equ 0

