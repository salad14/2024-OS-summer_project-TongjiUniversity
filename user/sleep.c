#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) { // 检查参数数量
        printf("Usage: sleep seconds\n");
        exit(1); // 参数错误，退出程序
    }

    int time = atoi(argv[1]); // 将字符串转换为整数
    sleep(time); // 调用 sleep 系统调用

    exit(0); // 正常退出
}

