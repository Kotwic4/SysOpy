#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <sys/wait.h>

pid_t parent_pid,child_pid;
int type;
int sig1;
int sig2;
int get_parent;
int get_child;
int parent_send;

void get_signal(int sig, siginfo_t *si, void *unused){
    get_parent++;
}

void get_child_signal(int sig, siginfo_t *si, void *unused){
    get_child++;
    //printf("%d\n", sig);
    union sigval val;
    val.sival_int = 0;
    val.sival_ptr = NULL;
    if(sig == sig1){
        switch (type) {
            case 1:
                kill(parent_pid, sig1);
                break;
            case 2:
                sigqueue(parent_pid, sig1, val);
                break;
            case 3:
                kill(parent_pid, sig1);
                break;
        }
    }
    else{
        _exit(get_child);
    }
}

void add_handle(int sig,void(*fun)(int,siginfo_t*,void*)){
    struct sigaction sa;
    sa.sa_flags =  SA_SIGINFO;
    sa.sa_sigaction = fun;
    sigaction(sig, &sa, NULL);
}

void print_score(){
    printf("Wyslane do potomka : %d\n"
                   "Odebrane przez potomka: %d\n"
                   "Odebrane przez rodzica: %d\n", parent_send,get_child,get_parent);
}

void quit(int sig){
    printf("\nOdebrano sygnał SIGINT\n");
    int status;
    kill(child_pid,SIGINT);
    waitpid(child_pid, &status, WUNTRACED | WCONTINUED);
    get_child = WEXITSTATUS(status);
    print_score();
    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Potrzeba 3 argumentow!\n");
        exit(EXIT_FAILURE);
    }
    int l = atoi(argv[1]);
    type = atoi(argv[2]);
    get_parent = 0;
    get_child = 0;
    parent_send = 0;
    if(type > 3 || type < 1) type = 1;
    if(type == 3){
        sig1 = SIGRTMIN+1;
        sig2 = SIGRTMIN+2;
    }
    else{
        sig1 = SIGUSR1;
        sig2 = SIGUSR2;
    }
    parent_pid = getpid();
    pid_t pid = fork();
    add_handle(sig1,get_child_signal);
    add_handle(sig2,get_child_signal);
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {            // Code executed by child
        while(1);
    } else {                    // Code executed by parent
        usleep(1);
        child_pid = pid;
        add_handle(sig1,get_signal);
        signal(SIGINT,quit);
        union sigval val;
        val.sival_int = 0;
        val.sival_ptr = NULL;
        for(int i = 0; i < l;i++) {
            switch (type) {
                case 1:
                    kill(pid,sig1);
                    break;
                case 2:
                    sigqueue(pid,sig1,val);
                    break;
                case 3:
                    sigqueue(pid,sig1,val);
                    break;
            }
            parent_send++;
            usleep(1);
        }
        usleep(1);
        switch (type) {
            case 1:
                kill(pid,sig2);
                break;
            case 2:
                sigqueue(pid,sig2,val);
                break;
            case 3:
                kill(pid,sig2);
                break;
        }
        parent_send++;
        int status;
        waitpid(pid, &status, WUNTRACED | WCONTINUED);
        get_child = WEXITSTATUS(status);
    }
    print_score();
    exit(EXIT_SUCCESS);
}
