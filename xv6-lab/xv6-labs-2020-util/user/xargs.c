#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs <command> [args...]\n");
        exit(1);
    }

    char buf;
    char arg_buf[1024];  // 扩大缓冲区
    char *args[MAXARG];
    int arg_count = 0;
    int pos = 0;

    // 初始化基本命令参数
    for (int i = 1; i < argc; i++) {
        args[arg_count++] = argv[i];
    }

    while (read(0, &buf, 1) > 0) {
        if (buf == '\n') {
            if (pos > 0) {
                arg_buf[pos] = 0;  // 终止当前参数
                args[arg_count++] = arg_buf;
                pos = 0;
            }
            
            // 安全检测
            if (arg_count >= MAXARG) {
                fprintf(2, "xargs: too many arguments\n");
                exit(1);
            }
            args[arg_count] = 0;  // exec要求NULL结尾
            
            if (fork() == 0) {
                exec(args[0], args);
                fprintf(2, "exec %s failed\n", args[0]);
                exit(1);
            } else {
                wait(0);
                // 重置参数计数器
                arg_count = argc - 1;
            }
        } else if (buf == ' ') {
            if (pos > 0) {
                arg_buf[pos] = 0;
                args[arg_count++] = arg_buf;
                pos = 0;
                
                // 缓冲区安全检查
                if (arg_count >= MAXARG - 1) {
                    fprintf(2, "xargs: too many arguments\n");
                    exit(1);
                }
            }
        } else {
            if (pos >= sizeof(arg_buf) - 1) {
                fprintf(2, "xargs: argument too long\n");
                exit(1);
            }
            arg_buf[pos++] = buf;
        }
    }

    // 处理最后一行（无换行符）
    if (pos > 0) {
        arg_buf[pos] = 0;
        args[arg_count++] = arg_buf;
        args[arg_count] = 0;
        
        if (fork() == 0) {
            exec(args[0], args);
            fprintf(2, "exec %s failed\n", args[0]);
            exit(1);
        } else {
            wait(0);
        }
    }
    
    exit(0);
}