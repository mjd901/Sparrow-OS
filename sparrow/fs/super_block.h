/*超级块，用于存放inode数组以及根目录地址等信息，每一个分区都有一个超级块*/
#ifndef __FS_SUPER_BLOCK_H
#define __FS_SUPER_BLOCK_H
#include "stdint.h"

/*超级块*/
struct super_block{
    uint32_t magic;//用于表示文件系统识别
    uint32_t sec_cnt;//本分区总共的扇区数
    uint32_t inode_cnt;//文件个数=inode结点数
    uint32_t part_lba_base;//本分区的起始lba地址

    uint32_t block_bitmap_lba;//块位图起始扇区
    uint32_t block_bitmap_sects;//扇区位图本身占用的扇区数量

    uint32_t inode_bitmap_lba; //i结点位图起始扇区lba地址，inode数量有限也需要使用位图管理
    uint32_t inode_bitmap_sects;//i结点位图占用的扇区数量


    uint32_t inode_table_lba; //i结点数组位图起始扇区lba地址
    uint32_t inode_table_sects;//i结点表占用的扇区数量


    uint32_t data_start_lba;//数据区开始的第一个扇区
    uint32_t root_inode_no;//根目录所在的i结点
    uint32_t dir_entry_size;//目录项大小

    uint8_t pad[460];//加上460个字节，刚好512字节一扇区

} __attribute__ ((packed));
#endif // !__FS_SUPER_BLOCK_H