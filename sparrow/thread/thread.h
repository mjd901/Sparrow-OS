#ifndef _THREAD_THREAD_H
#define _THREAD_THREAD_H
#include"stdint.h"

typedef void thread_func(void*);//参数是一个地址，指向自己的参数，返回为空

enum task_status{ //进程状态
    TASK_RUNNING, //运行中
    TASK_READY,     //就绪
    TASK_BLOCKED, //阻塞
    TASK_WAITING, //等待
    TASK_HANGING, //悬挂
    TASK_DIED     //死亡
};

struct intr_stack{
    uint32_t vec_no;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t error_code;
    void (*eip) (void);
    uint32_t cs;
    uint32_t eflags;
    void* esp;
    uint32_t ss;
};

struct thread_stack{
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edi;
    uint32_t esi;
    //上面四个寄存器在被调函数运行后，不应该被改变，他们归主调函数所有

    /*线程第一次执行时， eip 指向待调用的函数 kernel_thread
     其他时候， eip 是指向 switch_ to 的返回地址*/
    void (*eip) (thread_func* func,void* func_arg);
    void (*unused_retaddr);
    thread_func* function;
    void* func_arg;
};

//PCB
struct task_struct{
    uint32_t* self_kstack; //每个内核线程所使用的内核栈
    enum task_status status; //状态
    uint8_t priority; // 优先级别
    char name[16];
    uint32_t stack_magic;//梭的边界标记，用于检测梭的溢出
    
};

struct task_struct * thread_start(char* name,int prio,thread_func function,void* func_arg);
void init_thread(struct task_struct* pthread,char *name,int prio);
void thread_create(struct task_struct* pthread,thread_func function,void* func_arg);
void kernel_thread(thread_func* function,void* func_arg);
#endif