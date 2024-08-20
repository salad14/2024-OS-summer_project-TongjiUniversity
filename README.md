# 2024-OS-summer_project-TongjiUniversity

刘继业 2252752 Tongji University, 2024 Summer, Du BoWen 2024/8/26-8/30  
来源代码: [https://github.com/salad14/2024-OS-summer_project-TongjiUniversity](https://github.com/salad14/2024-OS-summer_project-TongjiUniversity)

各个实验的代码可切换不同的Branch查看

# Tools

### 在Windows上安装WSL

下载[Windows Subsystem for Linux](https://learn.microsoft.com/en-us/windows/wsl/install) 和 [Ubuntu on Windows](https://apps.microsoft.com/detail/9nblggh4msv6?hl=en-us&gl=us)。

在Windows中，您可以访问 `\\wsl$\` 目录下的所有WSL文件。例如，Ubuntu 20.04安装的主目录应该位于 `\\wsl$\Ubuntu-20.04\home\<username>\`。

```bash
C:\Users\17912>wsl --list --verbose
  NAME      STATE           VERSION
* Ubuntu    Stopped         2
```

### 软件源更新和环境准备

确保上一步已完成
启动Ubuntu，运行下列代码

```bash
$ sudo apt-get update && sudo apt-get upgrade
$ sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
```

### 测试安装

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

### 编译内核

下载XV6的源码
```bash
$ git clone git://github.com/mit-pdos/xv6-riscv.git
```

# Guidance

### 调试提示

1.理解 C 和指针：建议阅读《C 程序设计语言》（第二版）并做指针练习。如果不熟悉 C 的指针，会在实验中遇到很大困难。特别注意几个常见的指针习惯用法，比如 int *p = (int*)100 和 p[i] 等。

2.Git 使用：代码部分运行正常时，请用 Git 提交，以便出错时能回滚到之前的状态。

3.通过插入打印语句理解代码行为。如果输出太多，使用 script 命令记录输出并进行搜索。

4.使用 GDB 调试 xv6：在一个窗口运行 make qemu-gdb，在另一个窗口运行 gdb，设置断点并继续执行。当程序崩溃时，可以用 bt 获取回溯信息。如果内核崩溃或挂起，使用 GDB 查找问题所在。

5.QEMU 监控器：按 Ctrl-A C 进入 QEMU 控制台，使用 info mem 查询页面表。可用 cpu 命令选择核，或用 make CPUS=1 qemu 只启动一个核。

# Lab: Xv6 and Unix utilities

本实验将使您熟悉 xv6 及其系统调用。

### Boot xv6

1.获取实验室的 xv6 源代码并签出 util 分支：

```bash
$ git clone git://g.csail.mit.edu/xv6-labs-2021
Cloning into 'xv6-labs-2021'...
...
$ cd xv6-labs-2021
$ git checkout util
```
# png1

xv6-labs-2021 仓库与本书的仓库略有不同 XV6-RISCV;它主要添加一些文件。如果你好奇，可以通过下列命令查看git日志：
```bash
$ git log
```

Git 允许您跟踪对代码所做的更改。 例如，如果您完成了其中一项练习，并且想要检查您的进度，您可以提交更改通过运行：
```bash
$ git commit -am 'my solution for util lab exercise 1'
Created commit 60d2135: my solution for util lab exercise 1
 1 files changed, 1 insertions(+), 0 deletions(-)
```

2.构建并运行xv6

```bash
$ make qemu
riscv64-linux-gnu-gcc    -c -o kernel/entry.o kernel/entry.S
······
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 128M -smp 3 -nographic -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

xv6 kernel is booting

hart 1 starting
hart 2 starting
init: starting sh
$
```

如果在提示符下键入 ls，则应看到类似的输出 到以下内容：
```bash
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2226
xargstest.sh   2 3 93
cat            2 4 23656
echo           2 5 22488
forktest       2 6 13232
grep           2 7 26808
init           2 8 23312
kill           2 9 22424
ln             2 10 22272
ls             2 11 25824
mkdir          2 12 22544
rm             2 13 22528
sh             2 14 40552
stressfs       2 15 23520
usertests      2 16 150240
grind          2 17 37032
wc             2 18 24624
zombie         2 19 21800
console        3 20 0
```
这些是 mkfs 包含在 初始文件系统;大多数是您可以运行的程序。我们刚刚运行了其中一个：ls。

xv6没有ps命令，但是如果输入`Ctrl-p`，内核会打印每个进程的信息。如果你现在尝试一下，你会看到两行：一行是init，一行是sh。

要退出 qemu，请键入：`Ctrl-a x`.

# Sleep

### 实验目的
1.为xv6实现UNIX程序sleep。
2.实现的sleep应当按用户指定的ticks数暂停，其中tick是xv6内核定义的时间概念，即定时器芯片两次中断之间的时间。解决方案应该在文件user/sleep.c中。

### 实验步骤
1.在开始编码之前，请阅读 [xv6 book](https://pdos.csail.mit.edu/6.828/2021/xv6/book-riscv-rev2.pdf)的第1章，并查看`user/`中的其他程序（例如`user/echo.c`、`user/grep.c`和`user/rm.c`），了解如何获取传递命令行参数给程序。

2.在命令行中，您可以运行以下命令来打开文件并查看其内容：$ vim user/echo.c 可以使用任何文本编辑器打开文件，例如 Vim、Nano、Gedit 等。以Vim为例，在 Vim 编辑器中打开文件后，要退出并返回终端命令行界面，可以按照以下步骤操作：
```bash
1. 如果您处于编辑模式（Insert Mode），请按下 Esc 键，以确保切换到正常模式（Normal Mode）。

2. 在正常模式下，按 : 进入命令模式：
    保存文件并退出：wq 或 :x
    只保存文件：:w
    退出且不保存：:q!

3.使用 `cat` 命令来显示文件的内容。例如，运行以下命令来查看文件内容：`cat user/echo.c`
   `cat` 命令会将文件的内容直接输出到终端。

4.使用 less 命令：`less` 命令是一个分页查看器，用于逐页查看文件内容（使用空格键向下翻页，使用 b 键向上翻页，按下 q 键退出）。          例如，  运行以下命令来查看文件内容：`less user/echo.c`

5.总结：
  进入插入模式：i、a、o
  
  退出插入模式：Esc
  
  保存并退出：:wq
  
  不保存退出：:q!
  
  删除一行：dd
  
  复制一行：yy
  
  粘贴：p
  
  查找：/keyword
  
  撤销：u
```

3.   通过 `kernel/sysproc.c` 中的 `sys_sleep` 获取实现`sleep`系统调用的xv6内核代码；通过 `user/user.h`获取可从用户程序调用`sleep`的C语言定义；通过 `user/usys.S`获取从用户代码跳转到内核以实现`sleep`的汇编代码。

4.在程序中使用系统调用`sleep`，其中命令行参数以字符串形式传递，可以使用`atoi`（参见`user/ulib.c`）将其转换为整数。最后，需要确保`main`调用`exit()`以退出程序。此外如果用户忘记传递参数，sleep应打印错误信息。

   使用vim编辑器修改sleep程序：

   ```c
   int main(int argc, char *argv[])
   {
     if (argc < 2)
     {
       fprintf(2, "need a param");
       exit(1);
     }
     int time = atoi(argv[1]);
     sleep(time);
     exit(0);
   }
   ```

5.将编写好的睡眠程序添加到 Makefile 的 UPROGS 中;让 QEMU 编译你的程序，能够从 xv6 shell 运行它。

添加`sleep`目标程序：输入命令行`$ vim Makefile`打开`Makefile`文件，在 `Makefile` 中找到名为 `UPROGS` 的行，这是一个定义用户程序的变量。在 `UPROGS` 行中，添加 `sleep` 程序的目标名称：`$U/_sleep\`。

编译运行程序：在终端中，运行 `make qemu` 命令编译 xv6 并启动虚拟机，随后通过测试程序来检测sleep程序的正确性。

从xv6 shell运行程序：
```bash
 $ make qemu
...
init: starting sh
$ sleep 10
(nothing happens for a little while)
$
```

# 2png

### 实验中遇到的问题
为了使用sleep函数，需要包含相关头文件，我在阅读`user/user.h` 等头文件并结合控制台报错信息来确定需要在 `main` 函数中使用的头文件，最终成功调用 `sleep` 函数。

### 实验心得
实验要求我们阅读相关的代码，正确调用需要程序依赖相关的文件，理清参数传递和头文件依赖关系，避免参数传递出错或者头文件缺少导致的错误。还需要让系统支持sleep的调用，添加的makefile里来正确编译

# pingpong

### 实验目的
编写一个使用 UNIX 系统调用 ''ping-pong'' 的程序 一对管道上两个进程之间的字节，每个进程对应一个进程 方向。 父级应向子级发送一个字节; 孩子应打印“<PID>：已接收ping”， 其中 <pid> 是其进程 ID， 将管道上的字节写入父级， 并退出; 父级应从子级读取字节， 打印 “<PID>： Received Pong”， 并退出。 你 解决方案应该在文件 user/pingpong.c 中。

### 实验步骤
1.创建管道
```bash
   int p1[2], p2[2];  // 两个管道，p1用于父进程向子进程发送数据，p2用于子进程向父进程发送数据
    char buf[1];
    pipe(p1);
    pipe(p2);
```
2.使用fork创建子管道

3.使用read读管道数据，write写管道数据

4.使用getpid（）查找调用进程的id

5.重复之前的步骤并编译

6.运行并测试，在xv6 shell中运行该程序，输出结果如下：
```bash
$ pingpong
4: received ping
3: received pong
$
```
程序源代码:
```bash
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int p1[2], p2[2];  // 两个管道，p1用于父进程向子进程发送数据，p2用于子进程向父进程发送数据
    char buf[1];
    pipe(p1);
    pipe(p2);

    if (fork() == 0) {  // 子进程
        close(p1[1]);  // 关闭子进程中不需要的写端
        close(p2[0]);  // 关闭子进程中不需要的读端

        read(p1[0], buf, 1);  // 从父进程接收字节
        printf("%d: received ping\n", getpid());

        write(p2[1], "p", 1);  // 向父进程发送字节
        close(p1[0]);  // 关闭管道读端
        close(p2[1]);  // 关闭管道写端

        exit(0);
    } else {  // 父进程
        close(p1[0]);  // 关闭父进程中不需要的读端
        close(p2[1]);  // 关闭父进程中不需要的写端

        write(p1[1], "p", 1);  // 向子进程发送字节

        read(p2[0], buf, 1);  // 从子进程接收字节
        printf("%d: received pong\n", getpid());

        close(p1[1]);  // 关闭管道写端
        close(p2[0]);  // 关闭管道读端

        wait(0);  // 等待子进程结束
        exit(0);
    }
}
```
# png3
# 实验中遇到的问题
在最开始的代码中，我没有使用wait（）函数使父进程等待子进程，没有采用这样的同步机制会有可能导致二者的冲突。后来我在仔细的阅读了实验要求后发现其对这个进行了规则，因此需要加上这个，否则会导致子进程失去父进程不能导致进一步的处理

# 实验心得
本次实验让我认识到进程间通信在多进程编程中的重要性。管道作为通信机制，可以在父进程和子进程之间传递数据，实现数据的共享和交换。我们需要加入一些同步机制来同步进程的顺序，确保数据能正确交换和打印顺序。

# primes
### 实验目的
使用管道编写 Prime Sieve（主筛）的并发版本。这个想法这要归功于 Unix 管道的发明者 Doug McIlroy。

学习使用pipe和fork来设置管道。第一个进程将数字2到35输入管道。对于每个素数创建一个进程，该进程通过一个管道从左边的邻居读取数据，并通过另一个管道向右边的邻居写入数据。由于xv6的文件描述符和进程数量有限，第一个进程可以在35处停止。

### 实验步骤
1.创建父进程，父进程将数字2到35输入管道，此时不必创建其后所有进程，每一步迭代更新一对相对的父子进程（仅在需要时才创建管道中的进程）。

2.对于2-35中的每个素数创建一个进程，进程之间需要进行数据传递：该进程通过一个管道从左边的父进程读取数据，并通过另一个管道向右边子进程写入数据。

3.对于每一个生成的进程而言，当前进程最顶部的数即为素数；对每个进程中剩下的数进行检查，如果是素数则保留并写入下一进程，如果不是素数则跳过。

4.完成数据传递或更新时，需要及时关闭一个进程不需要的文件描述符（防止程序在父进程到达35之前耗尽xv6的资源）。

5.在数据传递的过程中，父进程需要等待子进程的结束，并回收共享的资源和数据等，即一旦第一个进程到达35，它应该等待直到整个管道终止，包括所有子进程、孙进程等。因此，主primes进程应该在所有输出都打印完毕，并且所有其他primes进程都退出后才退出。

6.编译程序，并且测试运行

源代码
```bash
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
sieve(int *left_pipe)
{
    int prime;
    if (read(left_pipe[0], &prime, sizeof(int)) == 0) {
        // 如果没有读取到内容，说明管道关闭了
        close(left_pipe[0]);
        exit(0);
    }
    
    printf("prime %d\n", prime);

    int right_pipe[2];
    pipe(right_pipe);

    if (fork() == 0) {
        // 子进程：继续处理筛选下一个质数
        close(right_pipe[1]); // 关闭子进程的写端
        sieve(right_pipe);
    } else {
        // 父进程：继续筛选
        close(right_pipe[0]); // 关闭父进程的读端
        int num;
        while (read(left_pipe[0], &num, sizeof(int)) > 0) {
            if (num % prime != 0) {
                write(right_pipe[1], &num, sizeof(int));
            }
        }
        close(left_pipe[0]);
        close(right_pipe[1]);
        wait(0); // 等待子进程结束
        exit(0);
    }
}

int
main(int argc, char *argv[])
{
    int left_pipe[2];
    pipe(left_pipe);

    if (fork() == 0) {
        // 子进程：启动筛选
        close(left_pipe[1]); // 关闭子进程的写端
        sieve(left_pipe);
    } else {
        // 父进程：向管道写入数字 2 到 35
        close(left_pipe[0]); // 关闭父进程的读端
        for (int i = 2; i <= 35; i++) {
            write(left_pipe[1], &i, sizeof(int));
        }
        close(left_pipe[1]); // 关闭写端，通知子进程没有更多数据了
        wait(0); // 等待子进程结束
        exit(0);
    }

    return 0;
}
```
# png4

### 实验中遇到的问题
fork()系统函数用于创建一个新的进程（子进程）作为当前进程（父进程）的副本，子进程会继承父进程的代码、数据、堆栈和文件描述符等资源的副本。子进程和父进程在 fork() 调用点之后的代码是独立执行的，并且拥有各自独立的地址空间。因此，父进程和子进程可以在 fork() 后继续执行不同的逻辑，实现并行或分支的程序控制流程。

在这个过程中，上一级的子会变成下一级的父，因此会能够解决其中的父子继承的关系。

### 实验心得
在这个函数中，我对管道数据传输和fork（）函数的调用有了新的理解。需要注意处理管道读写端的关闭和父子进程之间的关系，我对于并发编程有了深入的理解


# find
### 实验目的
编写一个简单版本的UNIX查找程序：查找所有文件 在具有特定名称的目录树中。您的解决方案应位于文件 user/find.c 中。

### 实验步骤
1.首先查看user/ls.c以了解如何读取目录。

2.main 函数完成参数检查和功能函数的调用：检查命令行参数的数量，如果参数数量小于 3，则输出提示信息并退出程序。否则，将第一个参数作为路径，第二个参数作为要查找的文件名称，调用 find 函数进行查找，并最后退出程序。

3.编写一个fmtname函数，通过查找路径中最后一个 '/' 后的第一个字符来获取文件的名称部分，然后与给定的名称进行比较。如果匹配则返回文件名。
```bash
char* fmtname(char *path) {
    static char buf[DIRSIZ+1];
    char *p;

    // 找到最后一个斜杠后的部分
    for(p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // 返回文件名
    if(strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    buf[strlen(p)] = 0;
    return buf;
}
````
4.编写find函数，用于递归的在给定的目录中查找带有特定名称的文件

如果是`T_FILE`则使用fmtname函数检查文件名称是否与给定的名称匹配，如果匹配则打印文件的路径。

如果是`T_DIR`目录。先检查是否超过缓冲区大小，不超过将路径拷贝在buf中并添加`/`。接下来，循环读取目录中的每个文件信息，注意不要递归到 '.' 和'..'，跳过当前目录 '.' 和上级目录 '..'。将文件名拷贝到 buf 中，并使用递归调用 find 函数在子目录中进行进一步查找。

5.编译后运行

# png5

### 实验中遇到的问题
在最开始时，我忽视了ls.c的输入仅仅需要一个文件地址参数,文件查找需要路径和文件名两个参数，没有对函数的输入进行对应的修改。导致报错，在我更改函数的输入之后这一点得到了解决

### 实验心得 
通过本次find实验的编写，我深入理解了文件系统中目录和文件之间的关系，学会了使用递归算法对目录树的深度遍历，并加强了自己在修正代码的能力，提高了对文件系统的理解与应用

# xargs
### 实验目的
编写一个UNIX xargs程序的简单版本：从标准输入中读取行，并为每一行运行一个命令，将行作为参数提供给命令。解决方案位于文件user/xargs.c中。
### 实验步骤
1.通过示例理解xarg的行为：
```bash
此处的命令是“echo bye”和附加的 参数是“你好也是”，使命令“echo bye 你好也”， 输出“再见你好”。
    $ echo hello too | xargs echo bye
    bye hello too
    $
  
请注意，UNIX 上的 xargs 会进行优化，它将一次向命令提供多个参数。我们不希望您进行此优化。要使 UNIX 上的 xargs 在此实验中按照我们希望的方式运行，请在将 -n 选项设置为 1 的情况下运行它。例如

    $ echo "1\n2" | xargs -n 1 echo line
    line 1
    line 2
    $
  ```

2.程序从标准输入中读取数据，每次读取一行，并将其作为参数传递给命令。

3.使用 fork() 创建子进程，然后在子进程中使用 exec() 执行命令，并将输入的行作为命令的附加参数。父进程则使用 wait() 等待子进程完成后再处理下一行输入。
```bash
while((n = read(0, buf, sizeof(buf))) > 0) {
        int m = 0;
        while(m < n) {
            int start = m;
            while(m < n && buf[m] != '\n') {
                m++;
            }
            buf[m] = 0; // 将换行符替换为字符串结束符
            args[base_args] = buf + start; // 将当前行作为参数添加
            args[base_args + 1] = 0; // 确保 args 以 NULL 结尾
            
            if(fork() == 0) { // 创建子进程
                exec(args[0], args); // 执行命令
                exit(0); // 防止 exec 失败后继续执行
            } else {
                wait(0); // 等待子进程完成
            }
            m++;
        }
    }
```

4.args 数组用于存储命令和附加的参数。初始的命令参数从 argv 中复制过来，标准输入的每一行被追加到 args 中作为命令参数。

5.编译运行

# png6

### 实验中遇到的问题
在最开始，由于我在最开始加了一个条件判断，导致有的时候无法读取到子文件，没能正确的输出hello。在查找到问题所在之后，我成功的处理了exec失败后继续执行的问题，正确的处理了文件之间的关系

### 实验心得
实验要求将输入按照空格拆分为多个参数，并将它们作为命令行参数传递给外部命令。我学会了如何处理命令行中的输入字符串，跳过空格，并将参数存储在适当的数据结构中。通过exec函数执行外部命令，我了解了进程创建替换的过程，学会了如何调试程序。

# Lab: system calls

在本实验中，您将向 XV6 添加一些新的系统调用，这将有所帮助 您了解它们的工作原理，并会让您接触到其中的一些 XV6 内核的内部结构。稍后将添加更多系统调用实验室。

在开始编码之前，请阅读 [XV6 book](https://pdos.csail.mit.edu/6.828/2021/xv6/book-riscv-rev1.pdf)，以及 第 4 章第 4.3 和 4.4 节，以及 相关源文件：

系统调用的用户空间代码是 在 user/user.h 和 user/usys.pl 中。
内核空间代码 是 kernel/syscall.h、kernel/syscall.c。
与进程相关的代码是 kernel/proc.h 和 kernel/proc.c。

### 实验前准备
若要启动实验室，请切换到 syscall 分支：
```bash
  $ git fetch
  $ git checkout syscall
  $ make clean
```

# System call tracing 
### 实验目的
旨在帮助了解系统调用跟踪的实现，以及如何修改 xv6 操作系统以添加新功能。我们需要添加一个有助于调试的新的trace系统调用。该功能包括创建一个名为 trace的系统调用，并将整数 "mask"作为参数。 "mask"的位数表示要跟踪哪些系统调用。通过实验，熟悉内核级编程，包括修改进程结构、处理系统调用和管理跟踪掩码。
### 实验步骤
首先，我们需要声明新的 trace 系统调用：
1.在 kernel/syscall.h 中声明系统调用号：在 kernel/syscall.h 中为 SYS_trace 添加一个新的条目。选择一个未使用的系统调用号
```bash
#define SYS_trace 22
```
2.在 user/user.h 中声明 trace 系统调用的原型：
```bash
int trace(int mask);
```
3.在 user/usys.pl 中添加 trace 系统调用的条目：
```bash
entry("trace");
```
接下来，我们需要在内核中实现处理 trace 系统调用的函数：
4.实现 sys_trace 函数，将 trace 掩码存储在进程的 proc 结构中：
```bash
uint64
sys_trace(void)
{
    int mask;

    if(argint(0, &mask) < 0)
        return -1;

    struct proc *p = myproc();
    p->trace_mask = mask;

    return 0;
}
```
5.在 proc 结构体中添加一个新的字段 trace_mask，用于存储 trace 掩码：
```bash
 int trace_mask;  // 用于存储 trace 掩码的新字段
```
6.在 fork 函数中，将 trace_mask 从父进程复制到子进程：
```bash
np->trace_mask = p->trace_mask;
```
7.在 kernel/syscall.c 中添加系统调用名称数组：

定义一个系统调用名称数组，方便打印系统调用的名称：
```bash
static char *syscall_names[] = {
    "", "fork", "exit", "wait", "pipe", "read",
    "kill", "exec", "fstat", "chdir", "dup",
    "getpid", "sbrk", "sleep", "uptime", "open",
    "write", "mknod", "unlink", "link", "mkdir",
    "close", "trace"
};
```
8.更新 syscall 函数，添加打印跟踪信息的逻辑：
```bash
void
syscall(void)
{
    int num;
    struct proc *p = myproc();

    num = p->trapframe->a7;

    if (num > 0 && num < NELEM(syscalls) && syscalls[num])
    {
        p->trapframe->a0 = syscalls[num]();
        // 打印跟踪信息
        if ((1 << num) & p->trace_mask) {
            printf("%d: syscall %s -> %d\n", p->pid, syscall_names[num],
                    p->trapframe->a0);
        }
    }
    else
    {
        printf("%d %s: unknown sys call %d\n",
               p->pid, p->name, num);
        p->trapframe->a0 = -1;
    }
}
```
9.编译运行
# png7
### 实验中遇到的问题
在最开始我编写完成后，出现了无法追踪全部系统调用的问题。后来发现问题出在没能正确的传递参数和数据上面，在查阅了相关的文档之后，解决了这个问题，避免了传参错误而导致的锁没能正确释放的问题。

### 实验心得
从这个实验中我学会了如何在xv6内核中添加新的系统调用，修改进程控制块和支持跟踪掩码。同时对于参数传递有了新的理解。

# Sysinfo
### 实验目的
在本实验中将添加一个系统调用 sysinfo，用于收集运行系统的信息。系统调用需要一个参数：指向 struct sysinfo 的指针（参见kernel/sysinfo.h）。内核应填写该结构体的字段：freemem 字段应设置为可用内存的字节数，nproc 字段应设置为状态不是 UNUSED 的进程数。
### 实验步骤
