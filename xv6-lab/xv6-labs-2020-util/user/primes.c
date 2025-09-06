#include "kernel/types.h"
#include "user/user.h"

void sieve(int read_fd, int depth) {
    // 添加深度检查后的描述符关闭
    if (depth > 10) {
        close(read_fd);
        exit(0);
    }

    int prime;
    // 添加EOF检测
    if (read(read_fd, &prime, sizeof(prime)) <= 0) {
        close(read_fd);
        exit(0);
    }
    printf("prime %d\n", prime);

    int pipe_fd[2];
    pipe(pipe_fd);

    if (fork() == 0) {
        close(pipe_fd[1]);
        sieve(pipe_fd[0], depth + 1);
        close(pipe_fd[0]);  // 子进程退出前关闭
        exit(0);
    } else {
        close(pipe_fd[0]);
        int num;
        while (read(read_fd, &num, sizeof(num)) > 0) {
            if (num % prime != 0) {
                write(pipe_fd[1], &num, sizeof(num));
            }
        }
        close(read_fd);
        close(pipe_fd[1]);
        wait(0);
    }
    exit(0);
}

int main() {
    int init_pipe[2];
    pipe(init_pipe);

    if (fork() == 0) {
        close(init_pipe[1]);
        sieve(init_pipe[0], 0);
        close(init_pipe[0]);
        exit(0);
    } else {
        close(init_pipe[0]);
        for (int i = 2; i <= 35; i++) {
            write(init_pipe[1], &i, sizeof(i));
        }
        close(init_pipe[1]);
        wait(0);
    }
    exit(0);
}