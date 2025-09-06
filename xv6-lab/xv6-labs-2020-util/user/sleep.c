#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {  // 修正参数检查
        fprintf(2, "Usage: sleep <ticks>\n");
        exit(1);
    }
    
    int ticks = atoi(argv[1]);
    if (ticks <= 0) {
        fprintf(2, "Error: Invalid tick count\n");
        exit(1);
    }
    
    sleep(ticks);
    exit(0);
}