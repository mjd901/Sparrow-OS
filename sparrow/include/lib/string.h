#ifndef _LIB_STRING_H
#define _LIB_STRING_H
#include"stdint.h"
/*将dst_起始的size个字节置为value*/
void memset(void * dst_,uint8_t value ,uint32_t size);
/*将src_起始的size个字节赋值给dst_*/
void memcpy(void* dst_,const void * src_ ,uint32_t size);
/*比较大小*/
int memcmp(const void* a_,const void* b_,uint32_t size);
/*const 表示不可以修改src_*/
char* strcpy(char* dst_,const char* src_);
/*获取长度*/
uint32_t strlen(const char* str);
/*
1.a>b ,返回1
2.a<b,返回-1
3.a==b,返回0
*/
int8_t strcmp(const char* a,const char*b);
/*从左到右查找字符a出现在字符串b中的首地址*/
char * strchr(const char* str,const uint8_t ch);
/*从右到左查找字符a出现在字符串b中的首地址*/
char * strrchr(const char* str,const uint8_t ch);
/*字符串src_拼接到dst_后面*/
char* strcat(char* dst_,const char* src_);
/*在字符串中查找字符ch出现的次数*/
uint32_t strchrs(const char* str,uint8_t ch);
#endif