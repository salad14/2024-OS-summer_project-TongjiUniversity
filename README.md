# 2024-OS-summer_project-TongjiUniversity

刘继业 2252752 Tongji University, 2024 Summer, Du BoWen 2024/8/26-8/30  

代码: [https://github.com/salad14/2024-OS-summer_project-TongjiUniversity](https://github.com/salad14/2024-OS-summer_project-TongjiUniversity)

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
# png TEST1
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
1.声明和定义系统调用
在 kernel/syscall.h 中为 SYS_sysinfo 添加一个新的系统调用编号：
```bash
#define SYS_sysinfo 23
```
由于 sysinfo 函数需要使用 struct sysinfo，我们需要在 user/user.h 中预先声明这个结构体，并添加 sysinfo 的函数原型：
```bash
struct sysinfo;

int sysinfo(struct sysinfo *);
```
在 user/usys.pl 中添加 sysinfo 的条目：
```bash
entry("sysinfo");
```
2.在内核中实现 sysinfo 系统调用，完成数据收集并将信息返回给用户空间。
在 kernel/sysproc.c 文件中实现 sysinfo 函数：
```bash
uint64
sys_sysinfo(void)
{
    struct sysinfo info;
    struct sysinfo *uinfo;

    if(argaddr(0, (uint64 *)&uinfo) < 0)
        return -1;

    // 获取空闲内存数
    info.freemem = free_mem();
    
    // 获取正在使用的进程数
    info.nproc = num_procs();

    // 将信息复制到用户空间
    if(copyout(myproc()->pagetable, (uint64)uinfo, (char *)&info, sizeof(info)) < 0)
        return -1;

    return 0;
}
```
在 kernel/kalloc.c 中实现一个函数，用于返回当前空闲的内存字节数：
```bash
uint64
free_mem(void)
{
    struct run *r;
    uint64 free = 0;
    acquire(&kmem.lock);
    for(r = kmem.freelist; r; r = r->next)
        free += PGSIZE;
    release(&kmem.lock);
    return free;
}
```
在 kernel/proc.c 中实现一个函数，用于返回当前正在使用的进程数：
```bash
int
num_procs(void)
{
    struct proc *p;
    int count = 0;
    for(p = proc; p < &proc[NPROC]; p++) {
        if(p->state != UNUSED)
            count++;
    }
    return count;
}
```
3.在Makefile中添加
$U/_sysinfotest

4.编译调试运行

### 实验中遇到的问题
本次实验的难点在于从系统中收集运行的信息。我参考kalloc（）和kfree（）函数，学习得知内核通过 kmem.freelist 的一个链表维护未使用的内存，然后让链表每个结点对应的页表大小相乘得到可用内存数。在编写的过程中，我疏忽了外部调用的声明，给编写造成了麻烦。

### 实验心得
本次实验我成功为系统添加了一个新的系统调用 sysinfo，实现了收集运行系统的信息。在完成的过程中，我学会了分析问题，参考其他函数来收集自己需要的信息，同时也学会了查阅资料，深入研究。这次实验教育我在编写代码的时候需要细心，不要忽视外部调用需要声明。

# test2

# Lab: page tables

在本实验中，您将浏览页表并将其修改为 加快某些系统调用的速度并检测已访问的页面。

kern/memlayout.h，用于捕获内存的布局。
kern/vm.c，其中包含大多数虚拟内存 （VM） 代码。
kernel/kalloc.c，其中包含用于分配和 释放物理内存。

若要启动实验室，请切换到 pgtbl 分支：

```bash
  $ git fetch
  $ git checkout pgtbl
  $ make clean
```

# Speed up system calls

### 实验目的
某些操作系统（例如 Linux）通过共享来加快某些系统调用的速度 用户空间和内核之间的只读区域中的数据。这消除了 执行这些系统调用时需要内核交叉。为了帮助您学习 如何将映射插入到页表中，您的首要任务就是实现这一点 对 xv6 中的 getpid（） 系统调用进行了优化。

在 xv6 操作系统中优化 getpid() 系统调用，通过在用户空间和内核之间共享一个只读页面，从而减少内核切换的开销。具体而言，当每个进程被创建时，需要在 USYSCALL（memlayout.h 中定义的虚拟地址）映射一个只读页面，并在页面的起始处存储一个 struct usyscall 结构体（也在 memlayout.h 中定义），并将其初始化为存储当前进程的 PID。实验要求的用户空间函数 ugetpid() 已经提供，并会自动使用 USYSCALL 映射。

### 实验步骤
1.在 kernel/proc.h 的proc 结构体中添加指针来保存这个共享页面的地址。
```bash
struct usyscall *usyscallpage;
```
2.在函数allocproc()中，为每一个新创建的进程分配一个只读页，使用 mappages() 来创建页表映射。

3.将 struct usyscall 结构放置在只读页的开头，并初始化其存储当前进程的 PID：

在 kernel/proc.c 的 proc_pagetable(struct proc *p) 中将这个映射（PTE）写入 pagetable 中。

4.在 kernel/proc.c 的 freeproc() 函数中，释放之前分配的只读页。

5.编译运行

# png8

### 实验中遇到的问题
在进行页面映射时，我遇到了编译出现的问题：
```bash
xv6 kernel is booting

panic: release
```

表明在某个地方，代码尝试释放一个未被持有的锁。后来经过调试发现，我在某处错误的使用了freeproc()释放了锁，后面又手动释放了锁，因此出现了冲突问题，在我修改了这个问题后得以解决

### 实验心得
这个实验使我更深入地理解了系统调用的工作原理以及它们是如何在用户空间和内核空间之间进行通信的。通过在每个进程的页表中插入只读页，我掌握了页表操作的方法，实现了用户空间和内核空间的共享

# Print a page table
### 实验目的
本实验的目标是编写一个函数 vmprint()，用于打印 RISC-V 页表的内容。通过这个实验，您将能够可视化页表的结构，并为以后的调试提供帮助。

### 实验步骤
1.在 kernel/vm.c 文件中定义 vmprint() 函数，该函数接受一个 pagetable_t 参数，并以特定格式打印该页表的内容。
```bash
void
vmprint(pagetable_t pagetable)
{
  printf("page table %p\n", pagetable);
  vmprint_walk(pagetable, 0, 0);
}

void
vmprint_walk(pagetable_t pagetable, int depth, uint64 va)
{
  for (int i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    if (pte & PTE_V) {
      uint64 pa = PTE2PA(pte);
      for (int j = 0; j < depth; j++) {
        printf(" ..");
      }
      printf("%d: pte %p pa %p\n", i, pte, pa);
      if ((pte & (PTE_R | PTE_W | PTE_X)) == 0) {
        pagetable_t next_level = (pagetable_t)PTE2PA(pte);
        vmprint_walk(next_level, depth + 1, va + (i << (12 + 9 * depth)));
      }
    }
  }
}
```
2.插入 vmprint() 调用：
在 exec.c 文件中找到 exec() 函数，在 return argc 语句之前插入 if(p->pid == 1) vmprint(p->pagetable); 代码，确保在初始化进程执行完 exec() 之后打印该进程的页表。

3.实现 vmprint() 函数：
使用递归遍历页表的每一层，并按照缩进的格式打印页表项（PTE）。只打印有效的 PTE，忽略无效的 PTE。
```bash
void
vmprint_walk(pagetable_t pagetable, int depth, uint64 va)
{
  for (int i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    if (pte & PTE_V) {  // 仅打印有效的 PTE
      uint64 pa = PTE2PA(pte);
      
      // 为每一层页表项打印适当的缩进
      for (int j = 0; j <= depth; j++) {
        printf(" ..");
      }
      
      printf("%d: pte %p pa %p\n", i, pte, pa);
      
      // 如果 PTE 指向下一级页表，递归打印
      if ((pte & (PTE_R | PTE_W | PTE_X)) == 0) {
        pagetable_t next_level = (pagetable_t)PTE2PA(pte);
        vmprint_walk(next_level, depth + 1, va + (i << (12 + 9 * depth)));
      }
    }
  }
}
```
4.使用 printf 打印 PTE 索引、PTE 位和物理地址，确保输出格式与实验要求一致。

5.编译运行

### 实验中遇到的问题
在最开始递归打印的时候，我没能正确处理好递归层级的问题，导致打印错误。后来我参考了freewalk函数的形式，成功的解决了问题。在理解页表的层次结构时，可能显得过于抽象，但通过观察源码、结合课程教材中的图 3-4和解释 vmprint() 输出的页表内容，我可以更清晰地了解每个级别的页表是如何映射虚拟地址到物理地址的。

### 实验心得
通过本次实验，我可以清晰地通过vmprint() 的输出来查看页表的层次结构，从根页表开始，逐级向下指向不同级别的页表页，最终到达最底层的页表页，其中包含了实际的物理页框映射信息。
这个实验加深了我对页表结构的理解，学会了如何在内核中操作位操作和宏定义，以及如何通过递归遍历页表来打印出整个页表的内容。

# Detecting which pages have been accessed
### 实验目的
一些垃圾回收器（一种自动内存管理形式）可以从中受益 从有关已访问（读取或写入）的页面的信息中。作为实验的一部分，您将向 XV6 添加一个新功能，用于检测并报告此问题 通过检查RISC-V页表中的访问位，将信息发送到用户空间。每当RISC-V硬件页面行走器解析时，它都会在PTE中标记这些位 TLB 未命中。
你的工作是实现 pgaccess（），这是一个报告已访问页面函数。系统调用采用三个参数。首先，它需要检查的第一个用户页面的起始虚拟地址。其次，它需要检查的页数。最后，它将用户地址带到缓冲区进行存储 将结果转换为 Bitmask（一种每页使用一位的数据结构，其中 第一页对应于最低有效位）。您将收到完整的 如果 PGCacess 测试用例在以下情况下通过，则为实验室的这一部分提供信用 运行 pgtbltest。

### 实验步骤
1.在kernel/riscv.h中定义访问位PTE_A。根据RISC-V手册，我们得知PTE_A位的值为0x040

2.解析系统调用sys_pgaccess()的三个参数
start_va：第一个用户页面的起始虚拟地址。
num_pages：要检查的页面数量。
user_mask：一个用户空间的地址，用于存储结果的位掩码。
通过 argaddr() 和 argint() 函数来解析传递进来的参数。如果参数解析失败，返回 -1 表示错误。
```bash
    uint64 start_va;
    int num_pages;
    uint64 user_mask;

    // 解析系统调用的参数
    if(argaddr(0, &start_va) < 0)
        return -1;
    if(argint(1, &num_pages) < 0)
        return -1;
    if(argaddr(2, &user_mask) < 0)
        return -1;
```

3.为了避免处理过大的内存范围，函数限制可以检查的最大页面数量。这里设定最大检查的页面数为 64。如果请求的页面数超过这个限制，则返回 -1。
```bash
  if(num_pages > 64)
        return -1;
```

4.遍历指定范围的页面并检查访问位。
walk() 函数用于获取页表项（PTE），其中包含了虚拟地址到物理地址的映射信息。
如果 pte 为 0，说明没有找到对应的页表项，跳过该页面。
如果 pte 存在且其访问位（PTE_A）被设置，表示该页面已被访问。此时，将对应位（1L << i）设置到 mask 中。
检查访问位后，清除 PTE_A 位，以便下次调用时可以正确检测新访问的页面。
```bash
    for(int i = 0; i < num_pages; i++) {
        pte_t *pte = walk(p->pagetable, start_va + i * PGSIZE, 0);
        if(pte == 0)
            continue;

        // 检查访问位并更新掩码
        if(*pte & PTE_A) {
            mask |= (1L << i);
            // 清除访问位
            *pte &= ~PTE_A;
        }
    }
```

5.将结果从内核空间复制到用户空间即可

6.实现完成，编译运行

### 实验中遇到的问题
在实验中如何清除PTE_A的访问位困扰了我很久，在多次查阅资料后得知，我可以采用检测pte的方式，如果 pte 为 0，说明没有找到对应的页表项，跳过该页面。如果 pte 存在且其访问位（PTE_A）被设置，表示该页面已被访问。此时，将对应位（1L << i）设置到 mask 中。
检查访问位后，清除 PTE_A 位，以便下次调用时可以正确检测新访问的页面。我成功的解决了问题

### 实验心得
通过这个实验，我学习了操作系统的内存管理机制，包括页表的结构和作用；理解了如何为进程分配页表，映射虚拟地址到物理地址，以及如何使用页表权限来实现不同的访问控制。我学会了如何查看代码文档来解决问题的能力。

# png9

# Lab: traps
本实验探讨了如何使用陷阱实现系统调用。 您将首先使用堆栈进行热身练习，然后您将实现用户级陷阱处理的示例。
若要启动实验室，请切换到陷阱分支：
```bash
 $ git fetch
  $ git checkout traps
  $ make clean
```

# RISC-V assembly
了解一些 RISC-V 汇编很重要。在 xv6 repo 中有一个文件 user/call.c。make fs.img 会对其进行编译，并生成 user/call.asm 中程序的可读汇编版本。

阅读 call.asm 中的 g ，f ，和 main 函数。

回答下面的问题：

**Q. 01**

> **Which registers contain arguments to functions? For example, which register holds 13 in main's call to `printf`?**

在 RISC-V 架构中，函数调用的前8个参数是通过寄存器传递的，分别是 a0 到 a7。具体来说：

a0：第1个参数
a1：第2个参数
依此类推
查看call.asm文件中的main函数可知，在 main 调用 printf 时，由寄存器 a2 保存 13。

# png 10

**Q. 02**

> **Where is the call to function f in the assembly code for main?
> Where is the call to g? (Hint: the compiler may inline functions.)**

查看call.asm文件中的f和g函数可知，函数 f 调用函数 g ；函数 g 使传入的参数加 3 后返回。
# png 11
编译器会进行内联优化，即一些编译时可以计算的数据会在编译时得出结果，而不是进行函数调用。查看 main 函数可以发现，printf 中包含了一个对 f 的调用。但是对应的会汇编代码却是直接将 f(8)+1 替换为 12 。这就说明编译器对这个函数调用进行了优化，所以对于 main 函数的汇编代码来说，其并没有调用函数 f 和 g ，而是在运行之前由编译器对其进行了计算。
# png 12

**Q. 03**

> **At what address is the function printf located?**
# png13

查阅得到其地址在 0x628。

**Q. 04**

> **What value is in the register ra just after the jalr to printf in main?**
0：使用 auipc ra,0x0 将当前程序计数器 pc 的值存入 ra 中。

34：jalr 1536(ra) 跳转到偏移地址 printf 处，也就是 0x630 的位置。

根据 reference1 中的信息，在执行完这句命令之后， 寄存器 ra 的值设置为 pc + 4 ，也就是 return address 返回地址 0x38。即jalr 指令执行完毕之后，ra 的值为 0x38.

# png14

**Q. 05**

> Run the following code.
>
> ```
> 	unsigned int i = 0x00646c72;
> 	printf("H%x Wo%s", 57616, &i);
> ```
>
> What is the output? [Here's an ASCII table]([ASCII Table - ASCII Character Codes, HTML, Octal, Hex, Decimal](https://www.asciitable.com/)) that maps bytes to characters.
>
> The output depends on that fact that the RISC-V is little-endian. If the RISC-V were instead big-endian what would you set `i` to in order to yield the same output? Would you need to change `57616` to a different value?
>
> [Here's a description of little- and big-endian](http://www.webopedia.com/TERM/b/big_endian.html) and [a more whimsical description](http://www.networksorcery.com/enp/ien/ien137.txt).

运行结果：打印出了 He110 World。

i 的值是 0x00646c72，它表示的字符串是 rl，因为 RISC-V 是小端序，所以字节顺序是反的。

57616 在十六进制中是 e110。

故输出 He110 World

**Q. 06**
> In the following code, what is going to be printed after `'y='`? (note: the answer is not a specific value.) Why does this happen?
>
> ```c
> 	printf("x=%d y=%d", 3);
> ```

printf 函数期望有两个参数：x 和 y。
由于代码中只传递了一个参数 3，所以 y 部分的输出将是未定义的行为，可能会打印一些垃圾值，因为第二个参数 y 没有提供。
可能导致 y 打印出一个随机的值或导致程序崩溃。

# Backtrace

### 实验目的
实现一个回溯（backtrace）功能，用于在操作系统内核发生错误时，输出调用堆栈上的函数调用列表。这有助于调试和定位错误发生的位置。

### 实验步骤
1.在 kernel/printf.c 中实现 backtrace() 函数
backtrace() 函数需要遍历当前堆栈帧，找到每个调用者的返回地址并将其打印出来。
```bash
void backtrace(void) {
    uint64 fp = r_fp();  // 获取当前帧指针
    printf("backtrace:\n");
    
    while(fp!= PGROUNDDOWN(fp)) {
	    printf("%p\n", *(uint64*)(fp-8));
	    fp = *(uint64*)(fp - 16);
  }
}
```
2.为了在其他地方调用 backtrace()，我们需要在 kernel/defs.h 中添加其原型。
```bash
void backtrace(void);
```

3.r_fp() 函数用于获取当前函数的帧指针（即 s0 寄存器的值）。这是一个内联汇编函数，可以帮助我们获取当前栈帧指针。
```bash
static inline uint64
r_fp()
{
    uint64 x;
    asm volatile("mv %0, s0" : "=r" (x));
    return x;
}
```
4.打开 kernel/sysproc.c 文件，并在 sys_sleep() 函数中调用 backtrace().这样，我们可以测试backtrace（）。

5.编译运行后，运行bttest

# png15

6.退出QEMU，使用addr2line 将输出的地址转换为具体的源代码行：
'''bash
addr2line -e kernel/kernel 0x00000000800020e6 0x0000000080001fac 0x0000000080001c96
'''

我们可以看到如下输出

# png16

7.为了在内核崩溃时获取调用栈信息，可以在 panic() 函数中调用 backtrace()。

打开 kernel/printf.c 文件，找到 panic() 函数，并添加对 backtrace() 的调用。

### 实验中遇到的问题

起初，我在 backtrace() 函数中使用了错误的格式化字符串，导致地址打印不正确。通过调试，我意识到在内核环境下需要更加小心地处理打印格式，并确保栈帧的指针在遍历过程中始终指向有效的内存区域。

此外，通过使用 addr2line 工具将返回地址映射回源码行，我成功地验证了 backtrace() 输出的正确性。这使我进一步体会到工具在调试和分析代码中的重要性。

### 实验心得
在这次实验中，我实现了 backtrace() 函数，用于在 RISC-V 架构下的 xv6 操作系统中获取并打印当前的函数调用栈。当内核遇到错误或者需要调试时，backtrace() 函数可以帮助我们跟踪并分析函数调用的历史，定位问题的根源。通过这一实验，我对操作系统内核的栈帧结构、函数调用的工作机制以及如何调试内核有了更深入的理解。
在实现 backtrace() 过程中，我学习并理解了 RISC-V 架构下栈帧的布局和使用方式。每个栈帧保存了函数调用的返回地址和前一个栈帧的指针（帧指针 fp），这使得可以沿着帧指针链逐步回溯，找到调用链中的每个函数。这种理解对我来说是非常宝贵的，因为它不仅是操作系统内核中重要的概念之一，也是调试和分析软件问题的基础。

# Alarm

### 实验目的
在本练习中，您将向 xv6 添加一个定期发出警报的功能 一个进程，因为它使用 CPU 时间。这对于计算受限可能很有用 想要限制它们消耗的 CPU 时间的进程，或者 想要计算但也希望定期进行一些计算的进程 行动。更一般地说，您将实现 用户级中断/故障处理程序;你可以使用类似的东西 例如，用于处理应用程序中的页面错误。您的解决方案 如果它通过了 alarmtest 和 userTests，则是正确的。

### 实验步骤
1.在 Makefile 中添加 alarmtest，使其可以作为用户程序进行编译。

2.定义 sigalarm 和 sigreturn 系统调用
在 user/user.h 中添加以下声明：
```bash
int sigalarm(int ticks, void (*handler)());
int sigreturn(void);
```

在 user/usys.pl 中添加以下行：
```bash
entry("sigalarm");
entry("sigreturn");
```

3.在 kernel/syscall.h 中添加 sigalarm 和 sigreturn 的系统调用号：
```bash
#define SYS_sigalarm  22
#define SYS_sigreturn 23
```
在 kernel/syscall.c 中添加对应的系统调用函数：
```bash
extern uint64 sys_sigalarm(void);
extern uint64 sys_sigreturn(void);

[SYS_sigalarm]  sys_sigalarm,
[SYS_sigreturn] sys_sigreturn,
```

4.在kernel/proc.h，将警报间隔和处理函数的指针存储在 proc 结构体的新字段中
```bash
uint64 handler_va;
int alarm_interval;
int passed_ticks;
struct trapframe saved_trapframe;
int have_return;
```
5.每次时钟中断发生时，硬件时钟会产生一个中断，这将在 usertrap() 函数中进行处理（位于 kernel/trap.c）。因此接下来需要修改 usertrap 函数，使得硬件时钟每滴答一次都会强制中断一次。
```bash
if(which_dev == 2) {
    struct proc *proc = myproc();
    if (proc->alarm_interval && proc->have_return) {
        if (++proc->passed_ticks == 2) {
        proc->saved_trapframe = *p->trapframe;
        proc->trapframe->epc = proc->handler_va;
        proc->passed_ticks = 0;
        proc->have_return = 0;
        }
    }
    yield();
}
```

6.完成第一步骤的工作后，我们可能遇到 alarmtest 在打印 "alarm!"后在 test0 或 test1 中崩溃，或者是 alarmtest（最终）打印 "test1 失败"，或者是 alarmtest 退出时没有打印 "test1 通过"。

因此，这一步涉及确保在处理完警报处理函数后，控制能够返回到被定时中断中断的用户程序指令处，同时保证寄存器的内容被恢复，以便用户程序能够在警报处理之后继续执行。最后，你需要在每次警报触发后 "重新激活" 定时器计数器，以便定时器定期触发处理函数的调用。

用户警报处理程序在完成后必须调用 sigreturn 系统调用。请看 alarmtest.c 中的 periodic 示例。这意味着您可以在 usertrap 和 sys_sigreturn 中添加代码，使用户进程在处理完警报后恢复正常。

我们可以编写一个符合要求的sys_sigreturn 函数：

```bash
uint64
sys_sigreturn(void)
{
  struct proc* proc = myproc();
  *proc->trapframe = proc->saved_trapframe;
  proc->have_return = 1; // true
  return proc->trapframe->a0;
}
```
7.测试
运行 alarmtest 并确保其能够正确地输出 "alarm!"。
# png17

运行 usertests 来确保你的修改没有影响到内核的其他部分。
# png18

### 实验中遇到的问题

在最后测试时出现了panic: balloc: out of blocks 的错误
```bash
test writebig: panic: balloc: out of blocks
```
表示在分配磁盘块时，文件系统中的可用块已经耗尽。这通常与文件系统的大小配置和测试过程中创建的文件数量有关。
我查阅资料得知writebig，会尝试写入大量数据以测试文件系统的极限。在块数不足的情况下，系统无法再为新数据分配块，从而触发 balloc 函数的错误。因此在我对宏定义#define FSSIZE   修改后，将文件系统的块数增加到了2000，解决了这个问题

### 实验心得
在本次实验中，我深入理解并实现了 xv6 操作系统中的一个关键功能：定期警报机制。通过为 xv6 添加 sigalarm 和 sigreturn 系统调用，我学到了如何在操作系统层面管理定时事件，并让用户空间的进程能够对这些事件做出响应。这种机制的实现对增强操作系统的灵活性和功能性具有重要意义，尤其是在处理实时系统或需要定期执行任务的场景中。

在实现过程中，最大的挑战是正确处理寄存器的保存和恢复，以及在处理程序执行完毕后确保进程能够从中断点继续无缝执行。这要求我对 xv6 的中断处理流程、进程状态管理以及系统调用机制有深入的理解。

通过这次实验，我不仅加深了对操作系统内部机制的认识，还提高了调试和解决复杂问题的能力，特别是在涉及底层系统编程的场景中。这些经验将对我今后处理更复杂的操作系统问题提供宝贵的帮助。
