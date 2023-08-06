#include"interrupt.h"
#include"init.h"
#include"print.h"
#include"memory.h"
void init_all(){
    idt_init();
    put_str("idt init done!");
    mem_init();
}