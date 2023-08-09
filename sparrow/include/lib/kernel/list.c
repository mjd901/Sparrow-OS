#include"list.h"
#include"interrupt.h"

/*初始化双向链表*/

void list_init(struct list* list){
    list->head.prev=NULL;
    list->head.next=&list->tail;
    list->tail.prev=&list->head;
    list->tail.next=NULL;
}

/*将元素elem插入到元素before前面*/
void list_insert_before(struct list_elem* before,struct list_elem* elem){
    /*公共资源，访问时只能有一个访问者修改，所以需要关闭中断*/
    enum intr_status old_status= intr_disable();
    before->prev->next = elem;
    elem->prev = before->prev;
    elem->next=before;
    before->prev=elem;
    intr_set_status(old_status);
}

/*将元素插入列表首，类似push*/
void list_push(struct list* plist,struct list_elem* elem){
    list_insert_before(plist->head.next,elem);
}

/*将元素插入队尾*/
void list_append(struct list* plist,struct list_elem* elem){
    list_insert_before(&plist->tail,elem);
}

/*使元素pelem脱离链表*/
void list_remove(struct list_elem* pelem){
    enum intr_status old_status= intr_disable();
    pelem->prev->next = pelem->next;
    pelem->next->prev=pelem->prev;
    intr_set_status(old_status);
}

/*pop 第一个elem*/

struct list_elem* list_pop(struct list* plist){
    struct list_elem* elem=plist->head.next;
    list_remove(elem);
}

/*查找节点elem 成功：true，失败：flase*/
bool elem_find(struct list* plist,struct list_elem* obj_elem){
    struct list_elem* elem=plist->head.next;
    while(elem!= &plist->tail){
        if(elem==obj_elem){
            return true;
        }
        elem=elem->next;
    }
    return false;
}

/*判断链表是否为空*/
bool list_empty(struct list* plist){
    return (plist->head.next==&plist->tail ? true:false);
}
/*返回链表链表长度*/
uint32_t list_len(struct list* plist){
    struct list_elem * elem= plist->head.next;
    uint32_t length = 0;
    while (elem!=&plist->tail)
    {
        length++;
        elem=elem->next;
    }
    return length;
}

/*..*/
struct list_elem* list_traversal(struct list* plist,function func,int arg){
    struct list_elem* elem=plist->head.next;
    /*如果队列为空则直接返回*/
    if(list_empty(plist)){
        return NULL;
    }
    while (elem!=&plist->tail)
    {
        /* code */
        if(func(elem,arg)){
            return elem;
        }
        elem=elem->next;
    }
    return NULL;
}

