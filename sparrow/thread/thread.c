#include"thread.h"
#include"string.h"
#include"global.h"
#include"memory.h"

#define PG_SIZE 4096  //一页有4096个字节

void kernel_thread(thread_func* function,void* func_arg){
    function(func_arg);
}
void thread_create(struct task_struct* pthread,thread_func function,void* func_arg){
    pthread->self_kstack -= sizeof(struct intr_stack);
    pthread->self_kstack -= sizeof(struct thread_stack);
    struct thread_stack* kthread_stack = (struct thread_stack *)pthread->self_kstack;//kthread_stack就是线程栈的起始地址
    kthread_stack->eip = kernel_thread;
    kthread_stack->function = function;
    kthread_stack->func_arg=func_arg;
    kthread_stack->ebp=kthread_stack->ebx=kthread_stack->edi=kthread_stack->esi=0;

}

//初始化pcb
void init_thread(struct task_struct* pthread,char *name,int prio){
    memset(pthread,0,sizeof(*pthread));
    strcpy(pthread->name,name);
    pthread->status=TASK_RUNNING;
    pthread->priority=prio;
    pthread->self_kstack=(uint32_t*)((uint32_t)pthread+PG_SIZE);//pcb栈顶
    pthread->stack_magic=0x19870916; //
}

struct task_struct * thread_start(char* name,int prio,thread_func* function,void* func_arg){
    struct task_struct* thread = get_kernel_pages(1); //pcb位于一页的首地址开始
    init_thread(thread,name,prio);
    thread_create(thread,function,func_arg);
    asm volatile("movl %0,%%esp;\
                pop %%ebp;\
                pop %%edx;\
                pop %%edi;\
                pop %%esi;\
                ret ;\
                "::"g"(thread->self_kstack):"memory");
    return thread;
}


