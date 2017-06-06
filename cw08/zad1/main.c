#include "common.h"
FILE * handle;
char * word_to_find;
int record_number;
pthread_t * threads;
int thread_number;
int word_len;

void * thread(void * args){
#ifdef ASYN
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
#endif
    struct Record * buff = calloc(record_number,RECORD_SIZE);
    while(1){
        int read = fread(buff, RECORD_SIZE, record_number, handle);
        if(read==0) {
            free(buff);
            pthread_exit(0);
        }
        for(int i=0;i<read;i++){
            int len = strlen(buff[i].text)-word_len;
            for(int j=0; j<=len;j++){
                if(strncmp(word_to_find,&buff[i].text[j],word_len)==0) {
                    printf("TID:%ld RID:%d\n", pthread_self(), buff[i].id);
#ifndef DETA
                    for(int k = 0; k < thread_number;k++){
                        if(pthread_equal(pthread_self(),threads[k])==0){
                            pthread_cancel(threads[k]);
                        }
                    }
                    free(buff);
                    pthread_exit(0);
#else
                    break;
#endif
                }
            }
        }
#ifdef SYNC
        sleep(0);
#endif
    }

}

int main(int argv, char ** argc){
    if(argv < 5){
        printf("Za malo argumentów(potrzeba 4)\n");
        exit(EXIT_FAILURE);
    }
    thread_number = atoi(argc[1]);
    char * filename = argc[2];
    record_number = atoi(argc[3]);
    word_to_find = argc[4];
    word_len = strlen(word_to_find);
    threads = calloc(thread_number,sizeof(pthread_t));
    if((handle = fopen(filename,"rb"))==NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<thread_number;i++){
        if(pthread_create(&threads[i], NULL, thread, NULL)==-1){
            perror("Create pthread");
            exit(EXIT_FAILURE);
        }
#ifdef DETA
        pthread_detach(threads[i]);
#endif
    }
#ifndef DETA
    for(int i=0;i<thread_number;i++){
        pthread_join(threads[i], NULL);
    }
#else
    while(1){
    }
#endif
    return 0;
}