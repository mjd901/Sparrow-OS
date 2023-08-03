#include "print.h"
#include"init.h"
void main(void)
{   
    put_str("hello sparrow! \n");
    init_all();
    asm volatile("sti");
    while(1); 
}
