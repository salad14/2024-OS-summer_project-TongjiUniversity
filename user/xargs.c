#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    char buf[512];
    char *args[MAXARG];
    int i, n = 0;

    // 将传递给 xargs 的初始参数复制到 args 中
    for(i = 1; i < argc; i++) {
        args[i-1] = argv[i];
    }
    int base_args = i - 1;

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

    exit(0);
}

