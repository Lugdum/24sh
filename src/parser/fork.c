#include "fork.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int ls_success(int flag)
{
    int success = 0;
    if (flag == NO_FLAG)
    {
        char *args[] = { "ls", NULL };
        int success = execvp("ls", args);
    }
    if (flag == DASH_L)
    {
        char *args[] = { "ls", "-l", NULL };
        int success = execvp("ls", args);
    }
    if (success != 0)
    {
        printf("Error executing ls command\n");
    }
    return success;
}

int new_process(int flag)
{
    pid_t pid;
    if (pid == 0)
    {
        printf("This is the child process\n");
        int success = ls_success(flag);
        printf("execvp() failed\n");
        if (success != 0)
        {
            printf("ls didn't take control of the fork\n");
            return 2;
        }
    }
    else if (pid > 0)
    {
        printf("This is the parent process\n");
        printf("Child process created with pid %d\n", pid);
    }
    else
    {
        printf("Fork failed\n");
        return 2;
    }
    return 0;
}