#include "kernel/types.h"
#include "user/user.h"

void primeSeive(int *oldFds)
{
    int newFds[2];
    pipe(newFds);
    int pid = fork();
    if (pid == 0)
    {
        // 需要注意的是，只有父进程和子进程都关闭了某个文件描述符，它才会真正被释放（计数才会为0）
        close(newFds[1]);
        int num;
        int numReveived = 0;
        while (read(newFds[0], &num, sizeof(int)) == sizeof(int))
        {
            fprintf(1, "prime %d\n", num);
            numReveived += 1;
            if (numReveived == 2)
            {
                primeSeive(newFds);
            }
        }
        close(newFds[0]);
    }
    else if (pid > 0)
    {
        int num;
        int numReveived = 0;
        int firstNum = 0;
        while (read(oldFds[0], &num, sizeof(int)) == sizeof(int))
        {
            numReveived += 1;
            if (numReveived == 1)
            {
                firstNum = num;
                fprintf(1, "prime %d\n", num);
            }
            else
            {
                if (num % firstNum != 0)
                {
                    write(newFds[1], &num, sizeof(int));
                }
            }
        }
        close(newFds[1]);
        close(oldFds[0]);
        wait(0);
    }
    else
    {
        fprintf(2, "create child process error!");
    }
    exit(0);
}

int main(int argc, char const *argv[])
{
    int fds[2];
    pipe(fds);
    for (int i = 2; i < 35; ++i)
    {
        write(fds[1], &i, sizeof(int));
    }
    close(fds[1]);
    primeSeive(fds);
    close(fds[0]);
    wait(0);
    exit(0);
}