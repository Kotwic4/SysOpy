#include "common.h"

int server_id;
int private_id;
int id;

void stop(){
    printf("Stopping client private server\n");
    if(msgctl(private_id,IPC_RMID,NULL) < 0){
        perror("failed to stop client private server");
        exit(EXIT_FAILURE);
    }
}

void connect(){
    server_id = msgget(SERVER_KEY,0);
    if(server_id == -1){
        perror("failed to connect to server");
        exit(EXIT_FAILURE);
    }
    private_id = msgget(IPC_PRIVATE,IPC_CREAT | IPC_EXCL | 0666);
    if(private_id == -1){
        perror("failed to start client private server");
        exit(EXIT_FAILURE);
    }
    struct msgbuf msg;
    msg.mtype = MSG_HELLO;
    msg.id = getpid();
    sprintf(msg.buf,"%d",private_id);
    msgsnd(server_id,&msg,MSG_SIZE,0);
    if(msgrcv(private_id,&msg,MSG_SIZE,0,0) == -1){
        perror("failed to get msg");
        stop();
        exit(EXIT_FAILURE);
    }
    id = msg.id;
    printf("Connected to server(id = %d) with id %d\n", server_id, id);
}

void quit(){
    printf("\nStopping Client\n");
    stop();
    exit(EXIT_SUCCESS);
}

void start(){
    printf("Starting client\n");
    connect();
    signal(SIGINT,quit);
}

int main(int argc, char *argv[])
{
    start();
    struct msgbuf msg;
    char type[10];
    msg.mtype = MSG_ECHO;
    msg.id = id;
    while(1){
        printf("$ ");
        scanf("%s", type);
        if(strcmp(type,"kill") == 0){
            msg.mtype = MSG_QUIT;
            strcpy(msg.buf,type);
            msgsnd(server_id,&msg,MSG_SIZE,0);
            quit();
        } else if(strcmp(type,"time") == 0){
            msg.mtype = MSG_TIME;
            strcpy(msg.buf,type);
        } else if(strcmp(type,"echo") == 0){
            msg.mtype = MSG_ECHO;
            scanf("%s",msg.buf);
        } else if(strcmp(type,"caps") == 0){
            msg.mtype = MSG_CAPS;
            scanf("%s",msg.buf);
        } else{
            printf("nieznany typ polecnia!\n");
            continue;
        }
        msgsnd(server_id,&msg,MSG_SIZE,0);
        if(msgrcv(private_id,&msg,MSG_SIZE,0,0) == -1){
            perror("failed to get msg");
            stop();
            exit(EXIT_FAILURE);
        }
        printf("%s\n",msg.buf);
    }
}
