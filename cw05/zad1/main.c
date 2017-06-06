#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>

const int N = 20;

void eval_line(char * line){
    char *program_line[N];
    char *args[N][6];
    pid_t pids[N];
    int status;
    int in[2];
    int out[2];
    program_line[0] = strtok(line,"|\n");
    int program_len = 1;
    while(program_len < 20 && (program_line[program_len] = strtok(NULL,"|\n")))
    {
        program_len++;
    }
    for(int i = 0; i< program_len; i++){
        args[i][0]= strtok(program_line[i]," ");
        if(args[i][0] != NULL) {
            int j = 1;
            do {
                args[i][j] = strtok(NULL, " \n");
                if (args[i][j] == NULL) {
                    break;
                }
                j++;
            } while (j < 5);
            args[i][j] = NULL;
        }
    }
    for(int i = 0; i < program_len; i++)
    {
        if(i == 0){
            in[0] = in[1] = STDIN_FILENO;
        }
        else{
            in[0] = out[0];
            in[1] = out[1];
        }
        if(i == program_len -1){
            out[0] = out[1] = STDOUT_FILENO;
        }
        else
        {
            pipe(out);
        }
        pid_t pid;
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {            // Code executed by child
            if(in[0] != STDIN_FILENO){
                dup2(in[0], STDIN_FILENO);
                close(in[1]);
            }
            if(out[1] != STDOUT_FILENO){
                dup2(out[1], STDOUT_FILENO);
                close(out[0]);
            }
            if(execvp(args[i][0], args[i]) == -1){
                char buf[30];
                sprintf(buf,"Bład w wykonaniu polecenia: %d ", i);
                perror(buf);
                exit(EXIT_FAILURE);
            }
        } else {                    // Code executed by parent
            if(in[0] != STDIN_FILENO){
                close(in[0]);
                close(in[1]);
            }
            pids[i] = pid;
        }
    }
    for(int i = 0; i < program_len; i++){
        if (waitpid(pids[i], &status, 0) == -1) {
            perror("failed to wait for child");
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    char *line = NULL;
    size_t len = 0;
    printf("$ ");
    fflush(stdout);
    while (getline(&line, &len,stdin) != -1)
    {
        eval_line(line);
        printf("$ ");
        fflush(stdout);
    }
    free(line);
    return 0;
}