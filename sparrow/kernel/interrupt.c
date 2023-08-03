#include"interrupt.h"
#include"global.h"
#include"stdint.h"
#include"print.h"
#include "io.h" //里面封装了一系列与端口操作的函数

/*--------------------------------------------------中断门描述符初始化开始------------------------------------------------------------------*/
#define IDT_DESC_CNT 0x21	   //支持的中断描述符个数33

//1.创建一个中断门描述符结构体
//2.创建中断门描述符表
//3.pic 初始化
//中断门描述符结构体
struct gate_desc{
    uint16_t func_offset_low_addr;//函数底地址
    uint16_t selector;//选择子
    uint8_t dcount;//该字段无用
    uint8_t attribute;//属性
    uint16_t func_offset_high_addr;//函数高地址
};

static void make_idt_desc(struct gate_desc * item,uint8_t attr,intr_handler function);
static struct gate_desc idt[IDT_DESC_CNT];
extern intr_handler intr_entry_table[IDT_DESC_CNT]; //汇编中的中断服务函数地址，是一个数组，数组里面存储着每一个数字

static void make_idt_desc(struct gate_desc * item,uint8_t attr,intr_handler function){
    item->func_offset_low_addr=(uint32_t)function & 0x0000ffff;
    item->selector=SELECTOR_K_CODE;//代码段
    item->dcount=0; //没用占位置
    item->attribute=attr;
    item->func_offset_high_addr=(uint32_t)function & 0xffff0000;
}

static void idt_desc_init(){
    for(int i=0;i<IDT_DESC_CNT;i++){
        make_idt_desc(&idt[i],IDT_DESC_ATTR_DPL0,intr_entry_table[i]);
    }
    put_str("idt_desc_init done!\n");
}


/*--------------------------------------------------中断门描述符初始化结束------------------------------------------------------------------*/

/*------------------------------------------------pic（中断控制器）8259A 初始化-------------------------------------------------------------------- */

#define PIC_M_CTRL 0x20	       // 这里用的可编程中断控制器是8259A,主片的控制端口是0x20
#define PIC_M_DATA 0x21	       // 主片的数据端口是0x21
#define PIC_S_CTRL 0xa0	       // 从片的控制端口是0xa0
#define PIC_S_DATA 0xa1	       // 从片的数据端口是0xa1

/* 初始化可编程中断控制器8259A */
static void pic_init(void) {

   /* 初始化主片 */
   outb (PIC_M_CTRL, 0x11);   // ICW1: 边沿触发,级联8259, 需要ICW4.
   outb (PIC_M_DATA, 0x20);   // ICW2: 起始中断向量号为0x20,也就是IR[0-7] 为 0x20 ~ 0x27.
   outb (PIC_M_DATA, 0x04);   // ICW3: IR2接从片. 
   outb (PIC_M_DATA, 0x01);   // ICW4: 8086模式, 正常EOI

   /* 初始化从片 */
   outb (PIC_S_CTRL, 0x11);	// ICW1: 边沿触发,级联8259, 需要ICW4.
   outb (PIC_S_DATA, 0x28);	// ICW2: 起始中断向量号为0x28,也就是IR[8-15] 为 0x28 ~ 0x2F.
   outb (PIC_S_DATA, 0x02);	// ICW3: 设置从片连接到主片的IR2引脚
   outb (PIC_S_DATA, 0x01);	// ICW4: 8086模式, 正常EOI

   /* 打开主片上IR0,也就是目前只接受时钟产生的中断 */
   outb (PIC_M_DATA, 0xfe);
   outb (PIC_S_DATA, 0xff);

   put_str("   pic_init done\n");
}


/*----------一般中断处理函数-----------------------------------*/
char* intr_name[IDT_DESC_CNT];		         //存储中断/异常的名字
intr_handler idt_table[IDT_DESC_CNT];	     // 定义中断处理程序数组.在kernel.S中定义的intrXXentry只是中断处理程序的入口,最终调用的是ide_table中的处理程序

/* 通用的中断处理函数,用于初始化,一般用在异常出现时的处理 */
static void general_intr_handler(uint8_t vec_nr) {
   if (vec_nr == 0x27 || vec_nr == 0x2f) {	//伪中断向量，无需处理。详见书p337
      return;		
   }
   put_str("int vector: 0x");
   put_int(vec_nr);
   put_char('\n');
}

/* 完成一般中断处理函数注册及异常名称注册 */
static void exception_init(void) {			    // 完成一般中断处理函数注册及异常名称注册
   int i;
   for (i = 0; i < IDT_DESC_CNT; i++) {

/* idt_table数组中的函数是在进入中断后根据中断向量号调用的,
 * 见kernel/kernel.S的call [idt_table + %1*4] */
      idt_table[i] = general_intr_handler;		    // 默认为general_intr_handler。
							    // 以后会由register_handler来注册具体处理函数。
      intr_name[i] = "unknown";				    // 先统一赋值为unknown 
   }
   intr_name[0] = "#DE Divide Error";
   intr_name[1] = "#DB Debug Exception";
   intr_name[2] = "NMI Interrupt";
   intr_name[3] = "#BP Breakpoint Exception";
   intr_name[4] = "#OF Overflow Exception";
   intr_name[5] = "#BR BOUND Range Exceeded Exception";
   intr_name[6] = "#UD Invalid Opcode Exception";
   intr_name[7] = "#NM Device Not Available Exception";
   intr_name[8] = "#DF Double Fault Exception";
   intr_name[9] = "Coprocessor Segment Overrun";
   intr_name[10] = "#TS Invalid TSS Exception";
   intr_name[11] = "#NP Segment Not Present";
   intr_name[12] = "#SS Stack Fault Exception";
   intr_name[13] = "#GP General Protection Exception";
   intr_name[14] = "#PF Page-Fault Exception";
   // intr_name[15] 第15项是intel保留项，未使用
   intr_name[16] = "#MF x87 FPU Floating-Point Error";
   intr_name[17] = "#AC Alignment Check Exception";
   intr_name[18] = "#MC Machine-Check Exception";
   intr_name[19] = "#XF SIMD Floating-Point Exception";

}

/*完成有关中断的所有初始化工作*/
void idt_init() {
   put_str("idt_init start\n");
   idt_desc_init();	   //调用上面写好的函数完成中段描述符表的构建
   exception_init();	// 异常名初始化并注册通常的中断处理函数
   pic_init();		  //设定化中断控制器，只接受来自时钟中断的信号

   /* 加载idt */
   uint64_t idt_operand = ((sizeof(idt) - 1) | ((uint64_t)(uint32_t)idt << 16));    //定义要加载到IDTR寄存器中的值 32位指针不能直接转64位地址，需要转位32int型，才能转64int型
   asm volatile("lidt %0" : : "m" (idt_operand));
   put_str("idt_init done\n");
}



