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

sem_t w_sem;
sem_t r_sem;
int readers = 0;
int n;
int *num;
int debug = 0;
int MAX = 100;

void Stake(sem_t* sem){
    if(sem_wait(sem)== -1){
        perror("Stake");
        exit(EXIT_FAILURE);
    }
}

void Sfree(sem_t* sem){
    if(sem_post(sem)== -1){
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
        Stake(&w_sem);
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
        Sfree(&w_sem);
        usleep(rand()%1000);
    }
}

void * r_thread(void * args){
    char buf[100];
    int a = ((int*)args)[0];
    while(1) {
        Stake(&r_sem);
        readers++;
        if(readers == 1){
            Stake(&w_sem);
        }
        Sfree(&r_sem);

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

        Stake(&r_sem);
        readers--;
        if(readers == 0){
            Sfree(&w_sem);
        }
        Sfree(&r_sem);
        usleep(rand()%1000);
    }
}

void stop(){
    logger("END");
    sem_destroy(&r_sem);
    sem_destroy(&w_sem);
    free(num);
    exit(EXIT_SUCCESS);
}

int main(int argv, char ** argc){
    if(argv < 4){
        printf("Za malo argumentów(potrzeba 3)\n");
        exit(EXIT_FAILURE);
    }
    srand(time(NULL));
    sem_init(&r_sem,0,1);
    sem_init(&w_sem,0,1);
    int r_number = atoi(argc[1]);
    int w_number = atoi(argc[2]);
    n = atoi(argc[3]);
    if(argv > 4 && strcmp(argc[4],"-i")==0) debug = 1;
    num = calloc(n,sizeof(int));
    for(int i = 0; i<n;i++) num[i] = 0;
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