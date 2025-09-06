#include "kernel/types.h"
#include "user/user.h"

int main() {
    int fd[2];
    char buf[4];
    pipe(fd);  // �����ܵ�

    if (fork() == 0) { 
        // �ӽ���
        read(fd[0], buf, sizeof(buf));  // �����ȴ�����������
        printf("%d: received ping\n", getpid());
        write(fd[1], "pong", 4);       // �ظ�������
        close(fd[1]);
        exit(0);
    } else { 
        // ������
        write(fd[1], "ping", 4); 
        close(fd[1]);   // �ر�д�ˣ������ӽ���read����
        wait(0);        // �ȴ��ӽ����˳�
        read(fd[0], buf, sizeof(buf)); 
        printf("%d: received pong\n", getpid());
        close(fd[0]);
    }
    exit(0);
}