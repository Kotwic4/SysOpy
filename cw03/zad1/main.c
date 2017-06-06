#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("Brak 1 argumentu(sciezka do pliku)!\n");
        return 1;
    }
    char* filename = argv[1];
    FILE* file = fopen(filename,"r");
    if(file == NULL){
        perror(filename);
        return 1;
    }

    int line_number = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
        line_number++;
        char *a[6];
        a[0]= strtok(line," \n");
        if(a[0] != NULL){

            if(a[0][0] == '#'){
                a[1] = strtok(NULL," \n");
                if(a[1] == NULL){
                    unsetenv(a[0]+1);
                }
                else{
                    setenv(a[0]+1,a[1],1);
                }
            }
            else{
                int i = 1;
                do{
                    a[i] = strtok(NULL," \n");
                    if(a[i] == NULL){
                        break;
                    }
                    i++;
                }
                while(i<5);
                a[i] = NULL;

                pid_t pid, w;
                int status;

                pid = fork();
                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                else if (pid == 0) {            // Code executed by child
                    if(execvp(a[0], a) == -1){
                        char buf[30];
                        sprintf(buf,"Bład w wykonaniu lini: %d " , line_number);
                        perror(buf);
                        exit(EXIT_FAILURE);
                    }

                } else {                    // Code executed by parent
                    w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
                    if (w == -1 || WEXITSTATUS(status) != 0) {
                        fclose(file);
                        exit(EXIT_FAILURE);
                    }
                }
            }

        }


    }

    free(line);
    fclose(file);
    return 0;
}