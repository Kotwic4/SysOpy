#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <sys/wait.h>

struct Node{
    pid_t pid;
    struct Node* next;
};

struct Queue{
    struct Node* head;
    struct Node* tail;
    int size;
};

struct Queue* init_queue(){
    struct Queue* q = malloc(sizeof (struct Queue));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

void push(struct Queue* q, pid_t pid){
    struct Node* node = malloc(sizeof (struct Node));
    node->pid = pid;
    node->next = NULL;
    if(q->size == 0){
        q->head = node;
    }
    else{
        q->tail->next = node;
    }
    q->tail = node;
    q->size++;
}

pid_t pop(struct Queue* q){
    pid_t pid = q->head->pid;
    struct Node* node = q->head;
    q->head = node->next;
    q->size--;
    free(node);
    return pid;
}

void free_queue(struct Queue* q){
    while(q->size > 0){
        pop(q);
    }
    free(q);
}

struct Queue* q;
pid_t parent_pid;
int n;
int k;
int child_called = 0;
int child_ended = 0;
pid_t* pid_tab;

void free_glob(){
    if(q){
        free_queue(q);
    }
    if(pid_tab){
        free(pid_tab);
    }
}

void child_main(){
    clock_t startTime,stopTime;
    int sig;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR2);
    srand(getpid());
    int sleep_time = rand()%(10000000);
    usleep(sleep_time);
    startTime = clock();
    kill(parent_pid, SIGUSR1);
    sigprocmask(SIG_SETMASK,&set,NULL);
    sigwait(&set, &sig);
    stopTime = clock();
    int sig_diff = (SIGRTMAX - SIGRTMIN +1);
    kill(parent_pid, SIGRTMIN + rand()%(sig_diff));
    _exit(stopTime - startTime);
}


void get_signal(int sig, siginfo_t *si, void *unused){
    printf("Sygnal:%d Pid:%d\n", sig, si->si_pid);
    fflush(stdout);
    if(sig == SIGUSR1){
        child_called++;
        push(q,si->si_pid);
    }
    else{
        int status;
        waitpid(si->si_pid, &status, WUNTRACED | WCONTINUED);
        printf("Koniec pid:%d status:%d\n",si->si_pid, WEXITSTATUS(status));
        fflush(stdout);
        child_ended++;
        for(int i = 0; i < n; i++){
            if(pid_tab[i] == si->si_pid) pid_tab[i] = 0;
        }
    }
}

void add_handle(int sig){
    struct sigaction sa;
    sa.sa_flags =  SA_SIGINFO;
    sa.sa_sigaction = get_signal;
    sigaction(sig, &sa, NULL);
}

void quit(int sig){
    printf("\nOdebrano sygnał SIGINT\n");
    fflush(stdout);
    for(int i = 0; i < n; i++)
    {
        if(pid_tab[i] != 0){
            kill(pid_tab[i], SIGINT);
        }
    }
    free_glob();
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("Potrzeba 3 argumentow!\n");
        exit(EXIT_FAILURE);
    }
    n = atoi(argv[1]);
    k = atoi(argv[2]);
    pid_tab = calloc(n,sizeof (pid_t));
    for(int i = 0; i < n; i++) pid_tab[i] = 0;
    q = init_queue();
    parent_pid = getpid();
    add_handle(SIGUSR1);
    for(int sig = SIGRTMIN; sig <= SIGRTMAX; sig++){
        add_handle(sig);
    }
    pid_t pid;
    for(int i = 0; i < n; i++){
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {            // Code executed by child
            child_main();
            return 0;
        } else {                    // Code executed by parent
            pid_tab[i] = pid;
        }
    }
    signal(SIGINT,quit);
    while(1){
        if(child_called >= k){
            while(q->size > 0){
                pid = pop(q);
                kill(pid, SIGUSR2);
            }
        }
        if(child_ended==n){
            break;
        }
    }
    free_glob();
    exit(EXIT_SUCCESS);
}