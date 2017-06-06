#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
    pid_t pid, w;
    int status;
    if(argc < 3){
        perror("za malo argumentow");
        exit(EXIT_FAILURE);
    }
    int n = atoi(argv[1]);
    for(int i = 0; i < n; i++){
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {            // Code executed by child
            if(execl("./client", "./client", argv[2],(char *) 0) == -1){
                perror("execl");
                exit(EXIT_FAILURE);
            }
        } else {                    // Code executed by parent

        }
    }
    while(n>0){
        w = waitpid(-1, &status, WUNTRACED | WCONTINUED);
        if (w == -1 || WEXITSTATUS(status) != 0) {
            exit(EXIT_FAILURE);
        }
        n--;
    }
    exit(EXIT_SUCCESS);
}
