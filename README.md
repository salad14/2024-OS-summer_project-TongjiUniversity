# 2024-OS-summer_project-TongjiUniversity

刘继业 2252752 Tongji University, 2024 Summer, Du BoWen 2024/8/26-8/30  
来源代码: [https://github.com/salad14/2024-OS-summer_project-TongjiUniversity](https://github.com/salad14/2024-OS-summer_project-TongjiUniversity)

各个实验的代码可切换不同的Branch查看

# Tools

## 在Windows上安装WSL

下载[Windows Subsystem for Linux](https://learn.microsoft.com/en-us/windows/wsl/install) 和 [Ubuntu on Windows](https://apps.microsoft.com/detail/9nblggh4msv6?hl=en-us&gl=us)。

在Windows中，您可以访问 `\\wsl$\` 目录下的所有WSL文件。例如，Ubuntu 20.04安装的主目录应该位于 `\\wsl$\Ubuntu-20.04\home\<username>\`。

```bash
C:\Users\17912>wsl --list --verbose
  NAME      STATE           VERSION
* Ubuntu    Stopped         2
```

## 软件源更新和环境准备

确保上一步已完成
启动Ubuntu，运行下列代码

```bash
$ sudo apt-get update && sudo apt-get upgrade
$ sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
```

## 测试安装

如果安装正常，运行下列命令行会显示如下的内容
```bash
$ qemu-system-riscv64 --version
QEMU emulator version 6.2.0 (Debian 1:6.2+dfsg-2ubuntu6.22)
Copyright (c) 2003-2021 Fabrice Bellard and the QEMU Project developers
```

```bash
$ riscv64-linux-gnu-gcc --version
riscv64-linux-gnu-gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0
Copyright (C) 2021 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

## 编译内核

下载XV6的源码
```bash
$ git clone git://github.com/mit-pdos/xv6-riscv.git
```

# Guidance

## 调试提示

1.理解 C 和指针：建议阅读《C 程序设计语言》（第二版）并做指针练习。如果不熟悉 C 的指针，会在实验中遇到很大困难。特别注意几个常见的指针习惯用法，比如 int *p = (int*)100 和 p[i] 等。

2.Git 使用：代码部分运行正常时，请用 Git 提交，以便出错时能回滚到之前的状态。

3.通过插入打印语句理解代码行为。如果输出太多，使用 script 命令记录输出并进行搜索。

4.使用 GDB 调试 xv6：在一个窗口运行 make qemu-gdb，在另一个窗口运行 gdb，设置断点并继续执行。当程序崩溃时，可以用 bt 获取回溯信息。如果内核崩溃或挂起，使用 GDB 查找问题所在。

5.QEMU 监控器：按 Ctrl-A C 进入 QEMU 控制台，使用 info mem 查询页面表。可用 cpu 命令选择核，或用 make CPUS=1 qemu 只启动一个核。

# Lab: Xv6 and Unix utilities

本实验将使您熟悉 xv6 及其系统调用。

## Boot xv6

1.获取实验室的 xv6 源代码并签出 util 分支：

```bash
$ git clone git://g.csail.mit.edu/xv6-labs-2021
Cloning into 'xv6-labs-2021'...
...
$ cd xv6-labs-2021
$ git checkout util
```
