#include "common.h"

int server_id;
int num_id;
int fifo_id;
int max_id;
int* num;
pid_t* fifo;
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

void stop(){
    semctl(server_id,0,IPC_RMID);
    shmdt(num);
    shmctl(num_id,IPC_RMID,NULL);
    shmdt(max);
    shmctl(max_id,IPC_RMID,NULL);
    shmdt(fifo);
    shmctl(fifo_id,IPC_RMID,NULL);
    free(ops);
    console_log("Barber close");
}

void quit(int sig){
    exit(EXIT_SUCCESS);
}

void start(int _max){
    signal(SIGINT,quit);
    atexit(stop);
    ops = calloc(4,sizeof(struct sembuf));
    server_id = semget(SERVER_KEY,SEMAFOR_NUM,IPC_CREAT|IPC_EXCL|0666);
    if(server_id == -1){
        perror("failed to create semafor");
        exit(EXIT_FAILURE);
    }
    num_id = shmget(NUM_KEY,sizeof(int),IPC_CREAT|IPC_EXCL|0666);
    if(num_id == -1){
        perror("failed to create num");
        exit(EXIT_FAILURE);
    }
    num = shmat(num_id,NULL,0);
    num[0] = 0;
    max_id = shmget(MAX_KEY,sizeof(int),IPC_CREAT|IPC_EXCL|0666);
    if(max_id == -1){
        perror("failed to create max");
        exit(EXIT_FAILURE);
    }
    max = shmat(max_id,NULL,0);
    max[0] = _max;
    fifo_id = shmget(FIFO_KEY,sizeof(pid_t)*max[0],IPC_CREAT|IPC_EXCL|0666);
    if(fifo_id == -1){
        perror("failed to create fifo");
        exit(EXIT_FAILURE);
    }
    fifo = shmat(fifo_id,NULL,0);
    Stake(SCHAIR);
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
        Stake(SFIFO);
        if(num[0] == 0){
            console_log("time to sleep");
            Stake(SBARBER);//razem
            Sfree(SFIFO);//razem
            Swait(SBARBER);
        }
        else{
            Sfree(SFIFO);
            Swait(SCHAIR);//razem
            Stake(SFIFO);//razem
            pid = fifo[0];
            sprintf(buf,"start of %d",pid);
            console_log(buf);
            Sfree(SFIFO);
            Stake(SFIFO);
            for(int i = 0;i<num[0]-1;i++){
                fifo[i]=fifo[i+1];
            }
            sprintf(buf,"end of %d",pid);
            console_log(buf);
            Sfree(SWORK);
            Sfree(SFIFO);
            Sfree(SCHAIR);
            Stake(SCHAIR);
        }
    }
}
