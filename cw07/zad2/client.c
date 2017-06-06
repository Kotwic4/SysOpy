#include "common.h"

sem_t * SBARBER;
sem_t * SFIFO;
sem_t * SQUEUE;
sem_t * SWORK;
sem_t * SCHAIR;
sem_t bla;

int* num;
pid_t* fifo;
int* max;

char buf_t[20];

sem_t *open_sem(char*name){
    sem_t* ans = sem_open(name,O_RDWR);
    if(ans == SEM_FAILED){
        perror("failed to create semafor");
        exit(EXIT_FAILURE);
    }
    else{
        return ans;
    }
}

void close_sem(sem_t* sem){
    sem_close(sem);
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

void* open_shm(char * name,int type){
    int id = shm_open(name,O_RDWR,0777);
    if(id == -1){
        perror("failed to open shm");
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

}

int Sget(sem_t* sem){
    int a;
    sem_getvalue(sem,&a);
    return a;
}


void stop(){
    close_sem(SBARBER);
    close_sem(SFIFO);
    close_sem(SQUEUE);
    close_sem(SWORK);
    close_sem(SCHAIR);
    sem_destroy(&bla);
    close_shm(fifo,FIFO_NAME,1);
    close_shm(max,MAX_NAME,0);
    close_shm(num,NUM_NAME,0);
    console_log("Client end");
}

void quit(){
    exit(EXIT_SUCCESS);
}

void start(){
    signal(SIGINT,quit);
    atexit(stop);
    SBARBER = open_sem(SBARBER_NAME);
    SFIFO = open_sem(SFIFO_NAME);
    SQUEUE = open_sem(SQUEUE_NAME);
    SWORK = open_sem(SWORK_NAME);
    SCHAIR = open_sem(SCHAIR_NAME);
    sem_init(&bla,0,0);
    num = (int*)open_shm(NUM_NAME,0);
    max = (int*)open_shm(MAX_NAME,0);
    fifo = (pid_t*)open_shm(FIFO_NAME,1);
    console_log("Client start");
}

void bla_(){
    Sfree(&bla);
    signal(SIGUSR1,bla_);
}

int main(int argc, char *argv[])
{
    if(argc < 2){
        perror("za malo argumentow");
        exit(EXIT_FAILURE);
    }
    int k = atoi(argv[1]);
    start();
    signal(SIGUSR1,bla_);
    while(k>0){
        Stake(SFIFO);
        if(Sget(SBARBER) == 0){
            console_log("wake up barber");
            Stake(SQUEUE);
            fifo[num[0]++]=getpid();
            Sfree(SBARBER);
            Sfree(SFIFO);
        }
        else{
            if(sem_trywait(SQUEUE)==-1){
                Sfree(SFIFO);
                console_log("exit because full");
                continue;
            }
            else{
                console_log("take seat");
                fifo[num[0]++]=getpid();
                Sfree(SFIFO);
            }
        }
        Stake(&bla);
        Swait(SWORK);
        Sfree(SCHAIR);
        console_log("exit because finish");
        k--;
    }
    quit();
}
