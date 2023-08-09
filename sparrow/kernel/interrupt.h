#ifndef __KERNEL_INTERRUPT_H
#define __KERNEL_INTERRUPT_H
#include"stdint.h"
typedef void* intr_handler; // 将intr_handler定义为void*同类型 32位模式下返回size位32bit
void idt_init(void);
enum intr_status
{
    INTR_OFF, // 中断关闭
    INTR_ON   // 中断打开
};
enum intr_status intr_get_status(void);//获取eflags -> if
enum intr_status intr_set_status(enum intr_status);//设置if
enum intr_status intr_enable(void);//开中断
enum intr_status intr_disable(void);//关闭中断
void register_handler(uint8_t vector_no, intr_handler function);

#endif
