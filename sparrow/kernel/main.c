#include "print.h"
#include"init.h"
#include"debug.h"
#include"string.h"
#include"memory.h"
#include"thread.h"

void k_thread_a(void*);
int main(void)
{   
    
    put_str("hello sparrow! \n");
    init_all();
    // asm("sti");
    thread_start("k_thread_a",31,k_thread_a,"argA"); //31是时间片
    while(1){asm("hlt");}; 
    return 0;
}

void k_thread_a(void* arg){ 
    char* para = arg;
    while(1){
        int i= 9999999;
        while(i--);
        put_str(para);
    }
}
