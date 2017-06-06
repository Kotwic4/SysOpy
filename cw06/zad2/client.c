#include "common.h"

mqd_t server_id;
mqd_t private_id;
int id;
char buffer[2048];
char name[20];

void stop(){
    printf("Stopping client private server\n");
    struct msgbuf msg;
    msg.mtype = MSG_END;
    msg.id = id;
    sprintf(msg.buf,"%s",name);
    sprintf(buffer,"%ld %ld %s\n",msg.id,msg.mtype,msg.buf);
    mq_send (server_id, buffer, sizeof (buffer), 1);
    if( mq_close(private_id) < 0 || mq_unlink(name)< 0){
        perror("failed to stop client private server");
        exit(EXIT_FAILURE);
    }
    if(mq_close(server_id) < 0){
        perror("failed to stop connection client to server");
        exit(EXIT_FAILURE);
    }
}

void connect(){
    server_id = mq_open(SERVERNAME,O_RDWR);
    if(server_id < 0){
        perror("failed to start server");
        exit(EXIT_FAILURE);
    }
    struct mq_attr attr;

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 2048;
    private_id = mq_open(name,O_RDWR| O_CREAT | O_EXCL, 0666, &attr);
    if(private_id == -1){
        perror("failed to start client private server");
        exit(EXIT_FAILURE);
    }

    struct msgbuf msg;
    msg.mtype = MSG_HELLO;
    msg.id = getpid();
    sprintf(msg.buf,"%s",name);
    sprintf(buffer,"%ld %ld %s\n",msg.id,msg.mtype,msg.buf);
    mq_send (server_id, buffer, sizeof (buffer), 1);
    if(mq_receive (private_id, buffer, sizeof (buffer), NULL) == -1){
        stop();
        perror("failed to get msg");
        exit(EXIT_FAILURE);
    }
    sscanf(buffer,"%ld %ld %s\n",&msg.id,&msg.mtype,msg.buf);
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
    sprintf(name,"/%d",getpid());
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
            sprintf(buffer,"%ld %ld %s\n",msg.id,msg.mtype,msg.buf);
            mq_send (server_id, buffer, sizeof (buffer), 1);
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
        sprintf(buffer,"%ld %ld %s\n",msg.id,msg.mtype,msg.buf);
        mq_send (server_id, buffer, sizeof (buffer), 1);
        if(mq_receive (private_id, buffer, sizeof (buffer), NULL) == -1){
            stop();
            perror("failed to get msg");
            exit(EXIT_FAILURE);
        }
        sscanf(buffer,"%ld %ld %s\n",&msg.id,&msg.mtype,msg.buf);
        printf("%s\n",msg.buf);
    }
}