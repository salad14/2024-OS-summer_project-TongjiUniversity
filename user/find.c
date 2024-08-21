#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

// 在 find 函数之前声明或定义 fmtname 函数
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

void find(char *path, char *target) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, O_RDONLY)) < 0) {
        printf("find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0) {
        printf("find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type) {
    case T_FILE:
        if(strcmp(fmtname(path), target) == 0) {
            printf("%s\n", path);
        }
        break;

    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            find(buf, target);
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if(argc < 3) {
        printf("Usage: find <path> <name>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}

