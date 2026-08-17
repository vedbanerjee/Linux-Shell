#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
        printf("Exiting with error code 1\n");
        exit(1);
    }
    else if (pid == 0)
    {
        printf("Using execl:\n");
        execl("/usr/bin/ls", "ls", "-l",NULL);
        perror("There is some issue..!");
    }
    else
    {
        wait(NULL);
        
    }
    return 0;
}