#ifndef _THREAD_THREAD_H
#define _THREAD_THREAD_H
#include"stdint.h"
#include"list.h"
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
//pcb
struct task_struct {
   uint32_t* self_kstack;	        // 用于存储线程的栈顶位置，栈顶放着线程要用到的运行信息
   enum task_status status;
   uint8_t priority;		        // 线程优先级
   char name[16];                   //用于存储自己的线程的名字

   uint8_t ticks;	                 //线程允许上处理器运行还剩下的滴答值，因为priority不能改变，所以要在其之外另行定义一个值来倒计时
   uint32_t elapsed_ticks;          //此任务自上cpu运行后至今占用了多少cpu嘀嗒数, 也就是此任务执行了多久*/
   struct list_elem general_tag;		//general_tag的作用是用于线程在一般的队列(如就绪队列或者等待队列)中的结点
   struct list_elem all_list_tag;   //all_list_tag的作用是用于线程队列thread_all_list（这个队列用于管理所有线程）中的结点
   uint32_t* pgdir;              // 进程自己页表的虚拟地址

   uint32_t stack_magic;	       //如果线程的栈无限生长，总会覆盖地pcb的信息，那么需要定义个边界数来检测是否栈已经到了PCB的边界
};

void thread_create(struct task_struct* pthread, thread_func function, void* func_arg);
void init_thread(struct task_struct* pthread, char* name, int prio);
struct task_struct* thread_start(char* name, int prio, thread_func function, void* func_arg);


void thread_init(void);
void schedule(void);
struct task_struct* running_thread(void);
#endif