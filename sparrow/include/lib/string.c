#include"string.h"
#include"global.h"
#include"debug.h"
/*将dst_起始的size个字节置为value*/
void memset(void * dst_,uint8_t value ,uint32_t size){
    //习惯上会判断地址是否为空
    ASSERT(dst_!=NULL);
    uint8_t* dst = (uint8_t*) dst_;
    while(size-- >0){
        *dst++=value;
    }
}
/*将src_起始的size个字节赋值给dst_ */
void memcpy(void* dst_,const void * src_ ,uint32_t size){
     ASSERT(dst_!=NULL&&src_!=NULL);
     uint8_t* dst=(uint8_t*) dst_;
     const uint8_t* src=(uint8_t*) src_;
     while (size-- >0)
     {
        *dst++=*src++;
     }
}
/*比较大小 相等返回0, a大于b返回1,否则返回-1*/ 
int memcmp(const void* a_,const void* b_,uint32_t size){
    uint8_t* a=(uint8_t*)a_;
    uint8_t* b=(uint8_t*)b_;
    ASSERT(a!=NULL||b!=NULL);
    while (size-- >0)
    {
        if(*a!=*b){
            return *a>*b?1:-1;
        }
        a++;
        b++;
    }
    return 0;
}
/*const 表示不可以修改src_*/
char* strcpy(char* dst_,const char* src_){
    ASSERT(dst_!=NULL||src_!=NULL);
    char * r=dst_;
    while ((*dst_++=*src_++));
    return r;
}

/*获取长度*/
uint32_t strlen(const char* str){
    ASSERT(str!=NULL);
    const char * p = str;
    while(*p++);
    return (p-str-1);
}

/*
1.a>b ,返回1
2.a<b,返回-1
3.a==b,返回0
*/
int8_t strcmp(const char* a,const char*b){
    ASSERT(a!=NULL&&b!=NULL);
    while (*a!=0&&*a==*b)
    {
        a++;
        b++;
    }
    return *a<*b?-1:*a>*b;
}

/*从左到右查找字符a出现在字符串b中的首地址*/
char * strchr(const char* str,const uint8_t ch){
    ASSERT(str!=NULL);
    while(*str!=0){
        if(*str==ch){
            return (char *)str;
        }
        str++;
    }
    return NULL;
}

/*从右到左查找字符a出现在字符串b中的首地址*/
char * strrchr(const char* str,const uint8_t ch){
    ASSERT(str!=NULL);
    const char * last_char=NULL;
    while(*str!=0){
        if(*str==ch){
            last_char = str;
        }
        str++;
    }
    return (char*)last_char;
}

/*字符串src_拼接到dst_后面*/
char* strcat(char* dst_,const char* src_){
    ASSERT(dst_!=NULL&&src_!=NULL);
    char* str=dst_;
    while(*str++);
    --str;
    while((*str++=*src_++));
    return dst_;
}

/*在字符串中查找字符ch出现的次数*/
uint32_t strchrs(const char* str,uint8_t ch){
    ASSERT(str!=NULL);
    uint32_t ch_cnt=0;
    while (*str!=0)
    {
        if(*str== ch){
            ch_cnt++;
        }
        str++;
    }
    return ch_cnt;
}

