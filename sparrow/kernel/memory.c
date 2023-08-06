#include"memory.h"
#include"stdint.h"
#include"print.h"

#define PG_SIZE 4096  //4kb 页表大小 = 0x1000 

/*
刚刚进入内核的时候将0x9f000作为栈顶，也就是说其实这里开始分配了一个main进程的pcb空间，
pcb需要占用一个自然页也就是说0Xxxxxx000，然后一个1kb的位图可以管理128mb的空间
这里设置4个位图也就是可以管理512mb的物理地址，虚拟机采用的是32mb内存暂时，所以减去4kb，等于0XC009A000 
*/
#define MEM_BITMAP_BASE 0XC009A000 
#define K_HEAP_START 0XC0100000 //;内核中堆的虚拟地址

//内存池结构，生成两个实例用于管理内核内存池和用户内存池
struct pool {
    struct bitmap pool_bitmap; //本内存池周到的位图结构， 用于管理物理内存
    uint32_t phy_addr_start;   //本内存池所管理物理内存的起始地址
    uint32_t pool_size;        //本内存池字节容量
};

struct pool kernel_pool,user_pool; //内核内存池和用户内存池
struct virtual_addr kernel_vaddr; //分配内核虚拟地址

/*初始化内存池*/

static void mem_pool_init(uint32_t all_mem){
    put_str("mem_pool_init start\n");
    uint32_t page_table_size = PG_SIZE * 256; //1MB以后页面占用的大小
    uint32_t used_mem = page_table_size +0x100000; //内核+页表使用的内存大小
    uint32_t free_mem = all_mem - used_mem;
    uint16_t all_free_pages = free_mem / PG_SIZE; // 计算有几个物理页 物理页=4kb
    uint16_t kernel_free_pages =  all_free_pages /2;       //内核使用的物里页数
    uint16_t user_free_pages = all_free_pages - kernel_free_pages; //用户物理页数

    uint16_t kbm_lenght = kernel_free_pages / 8; //内核位图长度，一位位图表示一页物理内存，这里表示需要多少字节位图表示内核的空间
    uint16_t ubm_lenght = user_free_pages / 8;

    uint32_t kp_start = used_mem; //内核内存池起始地址
    uint32_t up_start = kp_start+kernel_free_pages*PG_SIZE;

    kernel_pool.phy_addr_start = kp_start;
    user_pool.phy_addr_start =up_start;
    kernel_pool.pool_size = kernel_free_pages * PG_SIZE;
    user_pool.pool_size = user_free_pages * PG_SIZE;
    kernel_pool.pool_bitmap.btmp_bytes_len = kbm_lenght;
    user_pool.pool_bitmap.btmp_bytes_len=ubm_lenght;

    kernel_pool.pool_bitmap.bits = (void *) MEM_BITMAP_BASE; 
    user_pool.pool_bitmap.bits = (void * )(MEM_BITMAP_BASE+kbm_lenght);

    put_str("kernel_pool_bitmap_start:");
    put_int((int)kernel_pool.pool_bitmap.bits);
    put_str("kernel_pool_phy_addr_start:");
    put_int(kernel_pool.phy_addr_start);
    put_char('\n');

    put_str("user_pool_bitmap_start:");
    put_int((int)user_pool.pool_bitmap.bits);
    put_str("user_pool_phy_addr_start:");
    put_int(user_pool.phy_addr_start);
    put_char('\n');

    //将位图置0
    bitmap_init(&kernel_pool.pool_bitmap);
    bitmap_init(&user_pool.pool_bitmap);

    //下面初始化内核虚拟地址的位图，按实际物理内存大小生成数组
    kernel_vaddr.vaddr_bitmap.btmp_bytes_len = kbm_lenght;
    kernel_vaddr.vaddr_bitmap.bits = (void *)(MEM_BITMAP_BASE+kbm_lenght+ubm_lenght);

    kernel_vaddr.vaddr_start =K_HEAP_START;
    bitmap_init(&kernel_vaddr.vaddr_bitmap);
    put_str("mem_pool_init done!");

}

/*内存管理初始化入口*/
void mem_init(){
    put_str("mem_init start\n");
    uint32_t mem_bytes_total = (*(uint32_t * )(0xb00));
    mem_pool_init(mem_bytes_total);
    put_str("mem_init done");
}