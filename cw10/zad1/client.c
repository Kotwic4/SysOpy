#include "common.h"

int port;
char *name;
char *address;
int sock = -1;

void init_web(){
    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock == -1){
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in web_addr;
    web_addr.sin_family = AF_INET;
    web_addr.sin_port = htons(port);

    if (inet_aton(address, &web_addr.sin_addr) == 0) {
        perror("inet_aton");
        exit(EXIT_FAILURE);
    }

    if(connect(sock,(struct sockaddr *)&web_addr,sizeof(web_addr)) == -1){
        perror("connect");
        exit(EXIT_FAILURE);
    }
}

void init_local(){
    sock = socket(AF_UNIX,SOCK_STREAM,0);
    if(sock == -1){
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_un local_addr;
    local_addr.sun_family = AF_UNIX;
    strcpy(local_addr.sun_path,address);
    if(connect(sock,(struct sockaddr *)&local_addr,sizeof(local_addr)) == -1){
        perror("connect");
        exit(EXIT_FAILURE);
    }
}

long eval(struct Operation operation){
   switch(operation.op){
       case '+':
           return operation.a + operation.b;
       case '-':
           return operation.a - operation.b;
       case '/':
           return operation.a / operation.b;
       case '*':
           return operation.a * operation.b;
       default:
           return 0;
   }
}

void stop(){
    if(sock != -1){
        struct Msg msg;
        msg.type = KILL_TYPE;
        send(sock,&msg,sizeof(msg),0);
        shutdown(sock,SHUT_RDWR);
        close(sock);
    }
}

void quit(){
    exit(EXIT_SUCCESS);
}

int main(int argv, char ** argc) {
    if(argv < 4){
        printf("Zbyt malo argumentow, potrzeba minimum 3\n");
        exit(EXIT_FAILURE);
    }
    name = argc[1];
    address = argc[3];
    if(strcmp(argc[2],"web")==0){
        if(argv < 5){
            printf("Zbyt malo argumentow, potrzeba 4 argumentow dla web\n");
            exit(EXIT_FAILURE);
        }
        port = atoi(argc[4]);
        init_web();
    } else{
        init_local();
    }
    signal(SIGINT,quit);
    atexit(stop);
    struct Msg msg;
    msg.type = HELLO_TYPE;
    strcpy(msg.buf,name);
    send(sock,&msg,sizeof(msg),0);
    while(1){
        if(recv(sock,&msg,sizeof(msg),0)==-1){
            perror("recv");
            exit(EXIT_FAILURE);
        }
        switch (msg.type){
            case OPERATION_TYPE:
                msg.type = RESULT_TYPE;
                struct Operation operation;
                sscanf(msg.buf,OPERATION_S,&operation.id, &operation.a, &operation.b, &operation.op);
                sprintf(msg.buf,"id: %ld result:%ld name: %s",operation.id,eval(operation),name);
                break;
            case PING_TYPE:
                msg.type = PONG_TYPE;
                break;
            case EXITS_TYPE:
                printf("Name in use\n");
                shutdown(sock,SHUT_RDWR);
                close(sock);
                sock = -1;
                exit(EXIT_FAILURE);
        }
        send(sock,&msg,sizeof(msg),0);
    }
}
