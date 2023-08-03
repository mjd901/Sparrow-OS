#include"interrupt.h"
#include"init.h"
#include"print.h"
void init_all(){
    idt_init();
    put_str("idt init done!");
}