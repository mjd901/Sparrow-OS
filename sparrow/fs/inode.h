#ifndef __INODE_H
#define __INODE_H
#include "stdint.h"
#include "global.h"
#include "list.h"
/*inode结构*/
struct inode{
    uint32_t i_no;//inode编号
    uint32_t i_size;//当inode是文件时，等于文件大小，若是目录等于目录项大小之后
    uint32_t i_open_cnts;//记录此文件被打开的次数,用于关闭文件回收时使用
    bool write_deny;//写文件不能并行，进程写文件前检查此标识

    uint32_t i_sectors[13];//0~11 是直接块，12用来存储一级间接块指针

    struct list_elem inode_tag;
};
#endif // !__INODE_H