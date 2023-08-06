#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H
#include"stdint.h"
#include"bitmap.h"
/*虚拟地址池*/
struct virtual_addr{
    struct bitmap vaddr_bitmap;//虚拟地址位图结构
    uint32_t vaddr_start;//虚拟地址起始地址
};
/*内存标志*/
enum pool_flags{
    PF_KERNEL=1,
    PF_USER =2
};
#define PG_P_1 1
#define PG_P_0 0  
#define PG_RW_R 0 //读执行
#define PG_RW_W 2 //读写可执行
#define PG_US_S 0 //系统级别
#define PG_US_U 4 //用户级别
extern struct pool kernel_pool,user_pool; //定义用户内存池和内核池；
void mem_init();
#endif