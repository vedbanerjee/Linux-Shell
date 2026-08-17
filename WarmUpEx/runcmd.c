#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int n = argc - 1;
    if (n != 2)
    {
        printf("Incorrect number of arguments");
        return 1;
    }
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("Fork failed");
        printf("Exiting with error code 1\n");
        exit(1);
    }
    else if (pid == 0)
    {

        char *args[] = {argv[1], argv[2], NULL};
        execvp(argv[1], args);

        perror("execvp failed");
        exit(1);
    }
    else
    {
        wait(NULL);
    }
    return 0;
}