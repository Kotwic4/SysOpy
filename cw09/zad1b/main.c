#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

pthread_mutex_t w_mu;
pthread_mutex_t r_mu;
pthread_mutex_t f_mu;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int readers = 0;
int writers = 0;
int n;
int *num;
int debug = 0;
int MAX = 100;

void Stake(pthread_mutex_t* mutex){
    if(pthread_mutex_lock(mutex)== -1){
        perror("Stake");
        exit(EXIT_FAILURE);
    }
}

void Sfree(pthread_mutex_t* mutex){
    if(pthread_mutex_unlock(mutex)== -1){
        perror("Sfree");
        exit(EXIT_FAILURE);
    }
}


void logger(char * text){
    printf("TID:%ld %s\n", pthread_self(), text);
}

void * w_thread(void * args){
    char buf[100];
    while(1) {
        Stake(&w_mu);
        writers++;
        Sfree(&w_mu);
        Stake(&f_mu);
        int x = rand()%n+1;
        for(int i = 0 ; i < x; i++){
            int index = rand()%(n+1);
            int val = rand()%MAX;
            if(debug){
                sprintf(buf,"index:%d val:%d",index,val);
                logger(buf);
            }
            num[index] = val;
        }
        sprintf(buf,"write %d",x);
        logger(buf);
        Sfree(&f_mu);
        Stake(&w_mu);
        writers--;
        if(writers == 0){
            pthread_cond_broadcast(&cond);
        }
        Sfree(&w_mu);
        usleep(rand()%1000);
    }
}

void * r_thread(void * args){
    char buf[100];
    int a = ((int*)args)[0];
    while(1) {
        Stake(&w_mu);
        if(writers > 0){
            pthread_cond_wait(&cond, &w_mu);
        }
        Stake(&r_mu);
        readers++;
        if(readers == 1){
            Stake(&f_mu);
        }
        Sfree(&r_mu);
        Sfree(&w_mu);

        int x = 0;
        for(int i = 0; i < n; i++){
            if(num[i]%a == 0){
                x++;
                if(debug){
                    sprintf(buf,"index:%d val:%d",i,num[i]);
                    logger(buf);
                }
            }
        }
        sprintf(buf,"read %d",x);
        logger(buf);

        Stake(&r_mu);
        readers--;
        if(readers == 0){
            Sfree(&f_mu);
        }
        Sfree(&r_mu);
        usleep(rand()%1000);
    }
}

void stop(){
    logger("END");
    pthread_mutex_destroy(&w_mu);
    pthread_mutex_destroy(&r_mu);
    pthread_mutex_destroy(&f_mu);
    pthread_cond_destroy(&cond);
    free(num);
    exit(EXIT_SUCCESS);
}

int main(int argv, char ** argc){
    if(argv < 4){
        printf("Za malo argumentów(potrzeba 3)\n");
        exit(EXIT_FAILURE);
    }
    srand(time(NULL));
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&w_mu,&attr);
    pthread_mutex_init(&r_mu,&attr);
    pthread_mutex_init(&f_mu,&attr);
    pthread_mutexattr_destroy(&attr);
    int r_number = atoi(argc[1]);
    int w_number = atoi(argc[2]);
    n = atoi(argc[3]);
    if(argv > 4 && strcmp(argc[4],"-i")==0) debug = 1;
    num = calloc(n,sizeof(int));
    for(int i = 0; i<n;i++) num[i] = 2;
    int thread_number = r_number + w_number;
    pthread_t * threads = calloc(thread_number,sizeof(pthread_t));
    for(int i=0;i<r_number;i++){
        int*a = malloc(sizeof(int));
        a[0] = i+1;
        if(pthread_create(&threads[i], NULL,r_thread, a)==-1){
            perror("Create pthread");
            exit(EXIT_FAILURE);
        }
    }
    for(int i=0;i<w_number;i++){
        if(pthread_create(&threads[i+r_number], NULL,w_thread, NULL)==-1){
            perror("Create pthread");
            exit(EXIT_FAILURE);
        }
    }
    for(int i=0;i<thread_number;i++){
        pthread_join(threads[i], NULL);
    }
    stop();
}