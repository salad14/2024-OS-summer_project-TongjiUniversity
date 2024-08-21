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
