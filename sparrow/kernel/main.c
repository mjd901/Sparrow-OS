#include "print.h"
#include"init.h"
#include"debug.h"
#include"string.h"
#include"memory.h"
int main(void)
{   
    
    put_str("hello sparrow! \n");
    init_all();
    void* addr = get_kernel_pages(3);
    put_str("addr:\n");
    put_int((uint32_t)addr);
    while(1){asm("hlt");}; 
    /*
    0xc0100000~0xc01fffff->0x00100000~0x001fffff
    */
    return 0;
}
