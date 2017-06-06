#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

pthread_t * threads;
int thread_number;
int signal_number = SIGUSR1;
int type;

void logger(char * text){
    printf("TID:%ld %s\n", pthread_self(), text);
}

void * thread(void * args){
    logger("Start");
    if(type == 4 && pthread_equal(pthread_self(),threads[0])){
        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset,signal_number);
        int s = pthread_sigmask(SIG_BLOCK,&sigset,NULL);
        if(s != 0){
            perror("sigmask");
            exit(EXIT_FAILURE);
        }
    }
    if(type == 6){
        int a = 1;
        int b = 0;
        sleep(1);
        logger("zero");
        int c = a/b;
    } else{
        sleep(5);
    }
    logger("End");
    return 0;
}

void bla(){
    char buf[5];
    sprintf(buf,"%d",signal_number);
    logger(buf);
}

int main(int argv, char ** argc){
    if(argv < 4){
        printf("Za malo argumentów(potrzeba 3)\n");
        exit(EXIT_FAILURE);
    }
    thread_number = atoi(argc[1]);
    signal_number = atoi(argc[2]);
    type = atoi(argc[3]);
    if(type == 3 || type ==5){
        signal(signal_number,bla);
    }
    logger("Create");
    threads = calloc(thread_number,sizeof(pthread_t));
    for(int i=0;i<thread_number;i++){
        if(pthread_create(&threads[i], NULL, thread, NULL)==-1){
            perror("Create pthread");
            exit(EXIT_FAILURE);
        }
    }
    sleep(1);
    if(type < 6){
        if(type == 2){
            sigset_t sigset;
            sigemptyset(&sigset);
            sigaddset(&sigset,signal_number);
            int s = pthread_sigmask(SIG_BLOCK,&sigset,NULL);
            if(s != 0){
                perror("sigmask");
                exit(EXIT_FAILURE);
            }
        }
        logger("kill");
        if(type <= 3){
            kill(getpid(),signal_number);
        }
        else{
            printf("%ld\n",threads[0]);
            pthread_kill(threads[0],signal_number);
        }
    }
    logger("wait");
    for(int i=0;i<thread_number;i++){
        pthread_join(threads[i], NULL);
    }
    logger("end");
    return 0;
}