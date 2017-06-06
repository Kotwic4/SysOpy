#include "common.h"

int server_id;
int num_id;
int fifo_id;
int* num;
pid_t* fifo;
int max_id;
int* max;
struct sembuf* ops;

char buf_t[20];

void stop();

void console_log(char * buf){
    struct timespec time;
    if( clock_gettime( CLOCK_MONOTONIC, &time) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    } else{
        printf("[seconds:%ld nanoseconds:%ld pid:%d] %s\n",time.tv_sec,time.tv_nsec,getpid(),buf);
    }
}

void Stake(int type){
    sprintf(buf_t,"Stake %d",type);
    //console_log(buf_t);
    ops[0].sem_op=0;
    ops[0].sem_num=type;
    ops[1].sem_op=1;
    ops[1].sem_num=type;
    if(semop(server_id,ops,2)== -1){
        perror("Stake");
        exit(EXIT_FAILURE);
    }
}

void Sfree(int type){
    sprintf(buf_t,"Sfree %d",type);
    //console_log(buf_t);
    ops[0].sem_op=-1;
    ops[0].sem_num=type;
    if(semop(server_id,ops,1)== -1){
        perror("Sfree");
        exit(EXIT_FAILURE);
    }
}

void Swait(int type){
    sprintf(buf_t,"Swait %d",type);
    //console_log(buf_t);
    ops[0].sem_op=0;
    ops[0].sem_num=type;
    if(semop(server_id,ops,1)== -1){
        perror("Swait");
        exit(EXIT_FAILURE);
    }
}

int Sget(int type){
    return semctl(server_id,type, GETVAL);
}


void stop(){
    shmdt(num);
    shmdt(max);
    shmdt(fifo);
    free(ops);
    console_log("Client end");
}

void quit(){
    exit(EXIT_SUCCESS);
}

void start(){
    signal(SIGINT,quit);
    atexit(stop);
    ops = calloc(4,sizeof(struct sembuf));
    server_id = semget(SERVER_KEY,0,0);
    if(server_id == -1){
        perror("failed to get server");
        exit(EXIT_FAILURE);
    }
    num_id = shmget(NUM_KEY,0,0);
    if(num_id == -1){
        perror("failed to get num");
        exit(EXIT_FAILURE);
    }
    num = shmat(num_id,NULL,0);
    max_id = shmget(MAX_KEY,0,0);
    if(max_id == -1){
        perror("failed to get max");
        exit(EXIT_FAILURE);
    }
    max = shmat(max_id,NULL,0);
    fifo_id = shmget(FIFO_KEY,0,0);
    if(fifo_id == -1){
        perror("failed to get fifo");
        exit(EXIT_FAILURE);
    }
    fifo = shmat(fifo_id,NULL,0);
    console_log("Client start");
}

int main(int argc, char *argv[])
{
    if(argc < 2){
        perror("za malo argumentow");
        exit(EXIT_FAILURE);
    }
    int k = atoi(argv[1]);
    start();
    while(k>0){
        Stake(SFIFO);
        if(Sget(SBARBER) == 1){
            console_log("wake up barber");
            Sfree(SBARBER);
            fifo[num[0]++]=getpid();
            Sfree(SFIFO);
            Stake(SQUEUE);
        }
        else{
            if(num[0] >= max[0]){
                Sfree(SFIFO);
                console_log("exit because full");
                continue;
            }
            else{
                console_log("take seat");
                fifo[num[0]++]=getpid();
                Sfree(SFIFO);
                Stake(SQUEUE);
            }
        }
        Stake(SWORK);//razem
        Sfree(SCHAIR);//razem
        Swait(SWORK);
        Stake(SFIFO);//razem
        num[0]--;
        Sfree(SFIFO);
        Stake(SCHAIR);//razem
        Sfree(SQUEUE);
        console_log("exit because finish");
        k--;
    }
    quit();
}
