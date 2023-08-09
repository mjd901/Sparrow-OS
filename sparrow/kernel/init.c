#include"interrupt.h"
#include"init.h"
#include"print.h"
#include"memory.h"
#include "timer.h"
void init_all(){
    idt_init();
    put_str("idt init done!");
    timer_init();
    mem_init();
}