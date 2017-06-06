#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("Brak 1 argumentu(sciezka do pliku)!\n");
        return 1;
    }
    struct rlimit mem;
    struct rlimit cpu;

    if(argc <4){
        printf("Brak 2 i 3 argumentu(ograniczenia)!\n");
        return 1;
    }

    cpu.rlim_cur = cpu.rlim_max = atoi(argv[2]);
    mem.rlim_cur = mem.rlim_max = atoi(argv[3]) * 1000000;




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
                struct rusage rusage;
                int status;

                pid = fork();
                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                else if (pid == 0) {            // Code executed by child
                    setrlimit(RLIMIT_AS,&mem);
                    setrlimit(RLIMIT_CPU ,&cpu);
                    if(execvp(a[0], a) == -1){
                        char buf[30];
                        sprintf(buf,"Bład w wykonaniu lini: %d " , line_number);
                        perror(buf);
                        exit(EXIT_FAILURE);
                    }

                } else {                    // Code executed by parent
                    w = wait3(&status, WUNTRACED | WCONTINUED,&rusage);
                    if (w == -1 || WEXITSTATUS(status) != 0) {
                        fclose(file);
                        exit(EXIT_FAILURE);
                    }
                    else{
                        printf("Czas: user :%ld.%06ld system:%ld.%06ld\n",
                               rusage.ru_utime.tv_sec, rusage.ru_utime.tv_usec,
                               rusage.ru_stime.tv_sec, rusage.ru_stime.tv_usec);
                    }
                }
            }

        }


    }

    free(line);
    fclose(file);
    return 0;
}