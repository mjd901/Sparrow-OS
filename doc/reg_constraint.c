#include<stdio.h>
void main(){
    int int_a=1,int_b=2,out_sum;
    asm("addl %%ebx,%%eax":"=a"(out_sum):"a"(int_a),"b"(int_b));
    printf("int_a+int_b=%d\n",out_sum);
}
