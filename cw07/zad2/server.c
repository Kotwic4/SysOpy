#include "common.h"

sem_t * SBARBER;
sem_t * SFIFO;
sem_t * SQUEUE;
sem_t * SWORK;
sem_t * SCHAIR;

int* num;
pid_t* fifo;
int* max;

char buf_t[20];

sem_t *open_sem(char*name,int value){
    sem_t* ans = sem_open(name,O_CREAT|O_TRUNC|O_RDWR,0777,value);
    if(ans == SEM_FAILED){
        perror("failed to create semafor");
        exit(EXIT_FAILURE);
    }
    else{
        return ans;
    }
}

void close_sem(sem_t* sem,char*name){
    sem_close(sem);
    sem_unlink(name);
}


void console_log(char * buf){
    struct timespec time;
    if( clock_gettime( CLOCK_MONOTONIC, &time) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    } else{
        printf("[seconds:%ld nanoseconds:%ld pid:%d] %s\n",time.tv_sec,time.tv_nsec,getpid(),buf);
    }
}


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

void Swait(sem_t* sem){
    Stake(sem);
    Sfree(sem);
}

int Sget(sem_t* sem){
    int a;
    sem_getvalue(sem,&a);
    return a;
}

void* open_shm(char * name,int type){
    int id = shm_open(name,O_CREAT|O_TRUNC|O_RDWR,0777);
    size_t len;
    if(type == 0){
        len = NUM_SIZE;
    }
    else{
        len = FIFO_SIZE;
    }
    if(id == -1){
        perror("failed to open shm");
        exit(EXIT_FAILURE);
    }
    if(ftruncate(id,len) == -1){
        close(id);
        perror("failed to truncate shm");
        exit(EXIT_FAILURE);
    }
    void* ans;
    if(type == 0){
        ans = mmap(NULL,NUM_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,id,0);
    }
    else{
        ans = mmap(NULL,FIFO_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,id,0);
    }
    if(ans == (void *)-1){
        close(id);
        perror("failed to mmap");
        exit(EXIT_FAILURE);
    }
    close(id);
    return ans;
}

void close_shm(void*shm,char * name,int type){
    size_t len;
    if(type == 0){
        len = NUM_SIZE;
    }
    else{
        len = FIFO_SIZE;
    }
    if(munmap(shm,len)==-1){
        perror("failed to munmap");
        exit(EXIT_FAILURE);
    }
    if(shm_unlink(name)==-1){
        perror("failed to shm_unl");
        exit(EXIT_FAILURE);
    }

}

void stop(){
    close_sem(SBARBER,SBARBER_NAME);
    close_sem(SFIFO,SFIFO_NAME);
    close_sem(SQUEUE,SQUEUE_NAME);
    close_sem(SWORK,SWORK_NAME);
    close_sem(SCHAIR,SCHAIR_NAME);
    close_shm(fifo,FIFO_NAME,1);
    close_shm(max,MAX_NAME,0);
    close_shm(num,NUM_NAME,0);
    console_log("Barber close");
}

void quit(int sig){
    exit(EXIT_SUCCESS);
}



void start(int _max){
    signal(SIGINT,quit);
    atexit(stop);
    SBARBER = open_sem(SBARBER_NAME,1);
    SFIFO = open_sem(SFIFO_NAME,1);
    SQUEUE = open_sem(SQUEUE_NAME,_max);
    SWORK = open_sem(SWORK_NAME,1);
    SCHAIR = open_sem(SCHAIR_NAME,0);
    num = (int*)open_shm(NUM_NAME,0);
    num[0] = 0;
    max = (int*)open_shm(MAX_NAME,0);
    max[0]=_max;
    fifo = (pid_t*)open_shm(FIFO_NAME,1);
    console_log("Barber open");
}

int main(int argc, char *argv[])
{
    if(argc < 2){
        perror("za malo argumentow");
        exit(EXIT_FAILURE);
    }
    start(atoi(argv[1])+1);
    char buf[20];
    pid_t pid;
    while(1){
        if(Sget(SQUEUE) == max[0]){
            console_log("time to sleep");
            Stake(SBARBER);
            Swait(SBARBER);
        }
        Stake(SWORK);
        Stake(SFIFO);
        pid = fifo[0];
        for(int i = 0;i<num[0]-1;i++) {
            fifo[i] = fifo[i + 1];
        }
        num[0]--;
        Sfree(SQUEUE);
        Sfree(SFIFO);
        sprintf(buf,"start of %d",pid);
        console_log(buf);
        kill(pid,SIGUSR1);
        sprintf(buf,"end of %d",pid);
        console_log(buf);
        Sfree(SWORK);
        Stake(SCHAIR);
    }
}
