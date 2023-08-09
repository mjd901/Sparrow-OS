#include "memory.h"

#define PG_SIZE 4096 // 4kb 页表大小 = 0x1000

/*
刚刚进入内核的时候将0x9f000作为栈顶，也就是说其实这里开始分配了一个main进程的pcb空间，
pcb需要占用一个自然页也就是说0Xxxxxx000，然后一个1kb的位图可以管理128mb的空间
这里设置4个位图也就是可以管理512mb的物理地址，虚拟机采用的是32mb内存暂时，所以减去4kb，等于0XC009A000
*/
#define MEM_BITMAP_BASE 0XC009A000
#define K_HEAP_START 0XC0100000 //;内核中堆的虚拟地址

#define PDE_IDX(addr) ((addr & 0xffc00000) >> 22) // 取虚拟高地址10位
#define PTE_IDX(addr) ((addr & 0x003ff000) >> 12) // 取中10位地址

// 内存池结构，生成两个实例用于管理内核内存池和用户内存池
struct pool
{
    struct bitmap pool_bitmap; // 本内存池周到的位图结构， 用于管理物理内存
    uint32_t phy_addr_start;   // 本内存池所管理物理内存的起始地址
    uint32_t pool_size;        // 本内存池字节容量
};

struct pool kernel_pool, user_pool; // 内核内存池和用户内存池
struct virtual_addr kernel_vaddr;   // 分配内核虚拟地址

/*
在 pf 表示的虚拟内存池中申请 pg_cnt 个虚拟页，
成功则返回虚拟页的起始地址，失败则返回 NULL
*/
static void *vaddr_get(enum pool_flags pf, uint32_t pg_cnt)
{
    int vaddr_start = 0, bit_idx_start = -1;
    uint32_t cnt = 0;
    if (pf == PF_KERNEL)
    {
        bit_idx_start = bitmap_scan(&kernel_vaddr.vaddr_bitmap, pg_cnt);
        if (bit_idx_start == -1)
        {
            return NULL;
        }
        while (cnt < pg_cnt)
        {
            bitmap_set(&kernel_vaddr.vaddr_bitmap, bit_idx_start + cnt++, 1); // 将申请到的空间置为1；
        }
        vaddr_start = kernel_vaddr.vaddr_start + bit_idx_start * PG_SIZE;
    }
    else
    {
        // 用户空间页申请
    }
    return (void *)vaddr_start;
}
/*
在mpool指向的物理内存池中分配1个物理页
成功返回物理地址，失败返回NULL
*/
static void *palloc(struct pool *m_pool)
{
    int bit_idx = bitmap_scan(&m_pool->pool_bitmap, 1);
    if (bit_idx == -1)
    {
        return NULL;
    }
    bitmap_set(&m_pool->pool_bitmap, bit_idx, 1);
    uint32_t page_phyaddr = ((bit_idx * PG_SIZE) + m_pool->phy_addr_start);
    return (void *)page_phyaddr;
}
/*下面两个函数太妙了 淦*/
/*
得到虚拟地址addr对应的pde指针
*/
uint32_t *pde_ptr(uint32_t vaddr)
{
    uint32_t *pde = (uint32_t *)((0xfffff000) + PDE_IDX(vaddr) * 4);
    return pde;
}
/*
得到虚拟地址addr对应的pte指针
*/
uint32_t *pte_ptr(uint32_t vaddr)
{
    return (uint32_t *)(0xffc00000 + ((vaddr & 0xffc00000) >> 10) + PTE_IDX(vaddr) * 4);
}

/*建立虚拟和物理映射*/
static void page_table_add(void *_vaddr, void *_page_phyaddr)
{
    uint32_t vaddr = (uint32_t)_vaddr, page_phyaddr = (uint32_t)_page_phyaddr;
    uint32_t *pde = pde_ptr(vaddr);
    uint32_t *pte = pte_ptr(vaddr);
    if (*pde & 0x00000001)
    {
        ASSERT(!(*pte & 0X00000001));
        if (!(*pte & 0X00000001))
        { // 保证页表不存在
            *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
        }
        else
        {
            PANIC("pte repeat");
            *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
        }
    }
    else
    {                                                          // 页目录项不存在，所以要先创建页目录再创建页表项
        uint32_t pde_phyaddr = (uint32_t)palloc(&kernel_pool); // 分配一页内存，返回首地址，这里等于4kb，等于一个页表大小，
        *pde = (pde_phyaddr | PG_US_U | PG_RW_W | PG_P_1);

        // 清0,防止页表有杂乱数据 pte &0xfffff000 就是页表的起始地址
        memset((void *)((int)pte & 0xfffff000), 0, PG_SIZE);
        ASSERT(!(*pte & 0x00000001)); // 保证页表不存在
        *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
    }
}
/*分配 pg_cnt 个页空间，成功则返回起始虚拟地址，失败时返回 NULL*/
void *malloc_page(enum pool_flags pf, uint32_t pg_cnt)
{
    ASSERT(pg_cnt > 0 && pg_cnt < 3840); // 这里虚拟机内存容量为32mb，内核用户各16mb，这里算15mb，15mb/4kb = 3840；
    /**
    malloc__page 的原理是三个动作的合成：
    1 通过 vaddr get 在虚拟内存池中申请虚拟地址
    2 通过 palloc 在物理内存池中申请物理页
    3 通过 page table_add 将以上得到的虚拟地址和物理地址在页表中完成映射
    */
    void *vaddr_start = vaddr_get(pf, pg_cnt);
    if (vaddr_start == NULL)
    {
        return NULL;
    }
    uint32_t vaddr = (uint32_t)vaddr_start, cnt = pg_cnt;
    struct pool *mem_pool = pf & PF_KERNEL ? &kernel_pool : &user_pool;
    /*因为虚拟地址是连续的，但物理地址可以是不连续的，所以逐个做映射*/
    while (cnt-- > 0)
    {
        void *page_phyaddr = palloc(mem_pool); // 分配一页物理内存
        if (page_phyaddr == NULL)
        {
            return NULL;
        }
        page_table_add((void *)vaddr, page_phyaddr);
        vaddr += PG_SIZE; // 虚拟内存+一页偏移
    }
    return vaddr_start;
}

/*内核物理内存池中申请 1 页内存，
成功则返回其虚拟地址，失败则返回 NULL*/

void *get_kernel_pages(uint32_t pg_cnt)
{
    void *vaddr = malloc_page(PF_KERNEL, pg_cnt);
    if (vaddr != NULL)
    {
        memset(vaddr, 0, pg_cnt * PG_SIZE);
    }
    return vaddr;
}

/*初始化内存池*/
static void mem_pool_init(uint32_t all_mem)
{
    put_str("mem_pool_init start\n");
    uint32_t page_table_size = PG_SIZE * 256;       // 1MB以后页面占用的大小
    uint32_t used_mem = page_table_size + 0x100000; // 内核+页表使用的内存大小
    uint32_t free_mem = all_mem - used_mem;
    uint16_t all_free_pages = free_mem / PG_SIZE;                  // 计算有几个物理页 物理页=4kb
    uint16_t kernel_free_pages = all_free_pages / 2;               // 内核使用的物里页数
    uint16_t user_free_pages = all_free_pages - kernel_free_pages; // 用户物理页数

    uint16_t kbm_lenght = kernel_free_pages / 8; // 内核位图长度，一位位图表示一页物理内存，这里表示需要多少字节位图表示内核的空间
    uint16_t ubm_lenght = user_free_pages / 8;

    uint32_t kp_start = used_mem; // 内核内存池起始地址
    uint32_t up_start = kp_start + kernel_free_pages * PG_SIZE;

    kernel_pool.phy_addr_start = kp_start;
    user_pool.phy_addr_start = up_start;
    kernel_pool.pool_size = kernel_free_pages * PG_SIZE;
    user_pool.pool_size = user_free_pages * PG_SIZE;
    kernel_pool.pool_bitmap.btmp_bytes_len = kbm_lenght;
    user_pool.pool_bitmap.btmp_bytes_len = ubm_lenght;

    kernel_pool.pool_bitmap.bits = (void *)MEM_BITMAP_BASE;
    user_pool.pool_bitmap.bits = (void *)(MEM_BITMAP_BASE + kbm_lenght);

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

    // 将位图置0
    bitmap_init(&kernel_pool.pool_bitmap);
    bitmap_init(&user_pool.pool_bitmap);

    // 下面初始化内核虚拟地址的位图，按实际物理内存大小生成数组
    kernel_vaddr.vaddr_bitmap.btmp_bytes_len = kbm_lenght;
    kernel_vaddr.vaddr_bitmap.bits = (void *)(MEM_BITMAP_BASE + kbm_lenght + ubm_lenght);

    kernel_vaddr.vaddr_start = K_HEAP_START;
    bitmap_init(&kernel_vaddr.vaddr_bitmap);
    put_str("mem_pool_init done!");
}

/*内存管理初始化入口*/
void mem_init(void)
{
    put_str("mem_init start\n");
    uint32_t mem_bytes_total = (*(uint32_t *)(0xb00));
    mem_pool_init(mem_bytes_total);
    put_str("mem_init done");
}