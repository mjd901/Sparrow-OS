## 简介

本项目是一个os项目，编程语言主要为c和nasm汇编。使用nasm编译，并且使用makefile进行编译。主要文档在doc目录下。
项目组成
---sparrowos
-------doc     文档说明
-------image   文档图像
-------sparrow 项目源码

├─ doc
│   ├─ main
│   │   ├─ java
│   │   │   └─ com
│   │   │       └─ example
│   │   │           └─ YourClass.java
│   │   └─ resources
│   │       └─ config.properties
│   └─ test
│       └─ java
│           └─ com
│               └─ example
│                   └─ YourClassTest.java
├─ sparrow
│   ├─ README.md
│   └─ docs1.md
├─ build.gradle
└─ .gitignore

## 交叉编译
目标平台暂时为x86，gcc源码编译为i386版本

## 虚拟机

makefile支持brochs调试

## bootoader
主要设置cpu的工作模式，加载内核

## 内存管理
使用二级页表建立虚拟内存到物理内存的映射。使用内存池对内核空间和用户空间进行管理，对内存申请。使用位图标志内存使用的情况。

##  实现多线程
建立线程/进程的pcb数据结构，申请一个页的大小作为线程的使用。将pcb放在底地址，中断栈放在一页的高地址，线程栈放在中断栈的下面底地址处。使用链表结构实现线程创建，运行，管理

## 键盘、显示器
使用vga作为作为显示的设备，支持键盘的输入

## 用户进程
在多线程的基础上，新建整个页表，拥有4GB的资源，使用特权级为3的线程作为新的进程

## 硬盘驱动
- [X] TODO

## 文件管理
- [X] TODO
## shell实现
- [X] TODO
## 网络通信
- [X] TODO
