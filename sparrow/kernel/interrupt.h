#ifndef __KERNEL_INTERRUPT_H
#define __KERNEL_INTERRUPT_H
typedef void* intr_handler;		//将intr_handler定义为void*同类型 32位模式下返回size位32bit
void idt_init() ;
#endif

