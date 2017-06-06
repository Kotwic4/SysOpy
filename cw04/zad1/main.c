#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>


int i = 1;

void change(int sig){
    printf("\n");
    i *= -1;
}

void quit(int sig){
    printf("\nOdebrano sygnał SIGINT\n");
    exit(EXIT_SUCCESS);
}

int main(){
    signal(SIGINT,quit);
    struct sigaction sa;
    sa.sa_handler = change;
    sigaction(SIGTSTP, &sa, NULL);
    char c = 'A'-1;
    while(1){
        c += i;
        if(c > 'Z'){
            printf("\n");
            c = 'A';
        }else if(c < 'A'){
            printf("\n");
            c = 'Z';
        }
        printf("%c",c);
        fflush(stdout);
        usleep(100000);
    }
    return 0;
}