#include "print.h"
#include"init.h"
#include"debug.h"
#include"string.h"
int main(void)
{   
    put_str("hello sparrow! \n");
    init_all();
    asm volatile("sti");
    //test put_int(strchrs("hello",'l'));
    ASSERT(1==1);
   
    while(1){asm("hlt");}; 
    return 0;
}
