#ifndef _LIB_KERNEL_LIST_H
#define _LIB_KERNEL_LIST_H
#include"global.h"

struct list_elem{
    struct list_elem* prev;
    struct list_elem* next;
};

struct list{
    struct list_elem head;
    struct list_elem tail;
};

typedef bool (function)(struct list_elem*,int arg);

//用于计算一个结构体成员在结构体中的偏移量 //优秀
#define offset(struct_type,member_name) (int)(&(((struct_type*)0)->member_name))
//用于通过一个结构体成员地址计算出整个结构体的起始地址
#define elem_to_entry(struct_type,member_name,member_ptr) (struct_type*)((int)member_ptr-offset(struct_type,member_name))

void list_init(struct list* list);
/*将元素elem插入到元素before前面*/
void list_insert_before(struct list_elem* before,struct list_elem* elem);
void list_push(struct list* plist,struct list_elem* elem);
void list_append(struct list* plist,struct list_elem* elem);
void list_remove(struct list_elem* pelem);
struct list_elem* list_pop(struct list* plist);
bool elem_find(struct list* plist,struct list_elem* obj_elem);
bool list_empty(struct list* plist);
uint32_t list_len(struct list* plist);
struct list_elem* list_traversal(struct list* plist,function func,int arg);

#endif