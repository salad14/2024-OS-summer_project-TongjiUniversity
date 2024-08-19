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
