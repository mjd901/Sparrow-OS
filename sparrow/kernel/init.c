#include "interrupt.h"
#include "init.h"
#include "print.h"
#include "memory.h"
#include "timer.h"
#include "thread.h"
#include "console.h"
void init_all()
{
    idt_init();
    put_str("idt init done!");
    mem_init();
    thread_init();
    timer_init();
    console_init();
    keyboard_init();
}