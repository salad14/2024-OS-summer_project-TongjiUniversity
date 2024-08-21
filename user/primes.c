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
