#include "kernel/types.h"
#include "user/user.h"

int main() {
    int fd[2];
    char buf[4];
    pipe(fd);  // 创建管道

    if (fork() == 0) { 
        // 子进程
        read(fd[0], buf, sizeof(buf));  // 阻塞等待父进程数据
        printf("%d: received ping\n", getpid());
        write(fd[1], "pong", 4);       // 回复父进程
        close(fd[1]);
        exit(0);
    } else { 
        // 父进程
        write(fd[1], "ping", 4); 
        close(fd[1]);   // 关闭写端，触发子进程read返回
        wait(0);        // 等待子进程退出
        read(fd[0], buf, sizeof(buf)); 
        printf("%d: received pong\n", getpid());
        close(fd[0]);
    }
    exit(0);
}