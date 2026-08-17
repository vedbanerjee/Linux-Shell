#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


int main(){
    int pid = fork();
    if(pid<0){
        perror("Fork Failed");
        return 1;
    }
    else if(pid == 0){
        printf("Child process with PID: %d\n", (int)getpid());
        fflush(stdout);
    }
    else{
        pid_t hue = wait(NULL);
        printf("I am parent and my PID is %d\n", (int)getpid());
        printf("I am the parent of PID %d and i got return status as %d",  pid, hue);
    }

    return 0;
}