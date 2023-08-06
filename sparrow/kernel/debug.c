#include"debug.h"
#include"interrupt.h"
#include"print.h"

void panic_spin(char* filename,int line,const char* func,const char* condition){
    intr_disable();//断言时防止进程打断，所以关闭中断，关闭中断等于关闭进程，进程需要时钟中断来调节
    put_str("\n[error]");
    put_str("[filename]:");put_str(filename);put_str(" ");
    put_str("[line]:0x");put_int(line);put_str(" ");
    put_str("[func]:");put_str((char*)func);put_str("\n");
    put_str("[condition]:");put_str((char*)condition);put_str("\n");
    while(1){asm("hlt");};
}