#include "common.h"

#include <pthread.h>
#include <sys/epoll.h>

#define N 100
#define PING_SLEEP 5

struct Client{
    int fd;
    char name[MSG_BUF];
    int pong;
    struct sockaddr src_addr;
};

int port;
char *filename;
int local_sock;
int web_sock;
int epoll_fd;
struct epoll_event event;
struct Client clients[N];
int client_number = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_web(){
    web_sock = socket(AF_INET,SOCK_DGRAM,0);
    if(web_sock == -1){
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in web_addr;
    web_addr.sin_family = AF_INET;
    web_addr.sin_addr.s_addr = INADDR_ANY;
    web_addr.sin_port = htons(port);

    if(bind(web_sock,(struct sockaddr *)&web_addr,sizeof(web_addr)) == -1){
        perror("Bind");
        exit(EXIT_FAILURE);
    }

    event.data.fd = web_sock;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, web_sock, &event)==-1){
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
}

void init_local(){
    local_sock = socket(AF_UNIX,SOCK_DGRAM,0);
    if(local_sock == -1){
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_un local_addr;
    local_addr.sun_family = AF_UNIX;
    strcpy(local_addr.sun_path,filename);
    unlink(local_addr.sun_path);
    if(bind(local_sock,(struct sockaddr *)&local_addr,sizeof(local_addr)) == -1){
        perror("Bind");
        exit(EXIT_FAILURE);
    }
    event.data.fd = local_sock;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, local_sock, &event)==-1){
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
}

void add_client(int fd, char*name, struct sockaddr src_addr){
    int delete = 0;
    struct Msg msg;
    pthread_mutex_lock(&clients_mutex);
    if(client_number >= N){
        delete = 1;
    }else{
        for(int i = 0 ; i < client_number; i++){
            if(strcmp(clients[i].name,name)==0){
                delete = 1;
                break;
            }
        }
    }
    if(delete){
        msg.type = EXITS_TYPE;
        sendto(fd,&msg,sizeof(msg),0,&src_addr,sizeof(src_addr));
    }
    else{
        clients[client_number].fd = fd;
        clients[client_number].pong = -1;
        strcpy(clients[client_number].name,name);
        memcpy(&clients[client_number].src_addr,&src_addr,sizeof(src_addr));
        client_number++;
    }
    pthread_mutex_unlock(&clients_mutex);
}

void close_client(struct sockaddr src_addr){
    pthread_mutex_lock(&clients_mutex);
    for(int i = 0 ; i < client_number; i++){
        if(memcmp(&clients[i].src_addr,&src_addr,sizeof(src_addr))==0){
            for(int j = i+1; j < client_number; j++){
                i = j-1;
                clients[i].fd = clients[j].fd;
                clients[i].pong = clients[j].pong;
                strcpy(clients[i].name,clients[j].name);
                memcpy(&clients[i].src_addr,&clients[j].src_addr,sizeof(clients[j].src_addr));
            }
            client_number--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* ping_thread_main(void *args){
    struct Msg msg;
    msg.type = PING_TYPE;
    while(1){
        pthread_mutex_lock(&clients_mutex);
        for(int i = 0; i< client_number; i++){
            clients[i].pong = 0;
            sendto(clients[i].fd,&msg,sizeof(msg),0,&clients[i].src_addr,sizeof(clients[i].src_addr));
        }
        pthread_mutex_unlock(&clients_mutex);
        sleep(PING_SLEEP);
        pthread_mutex_lock(&clients_mutex);
        for(int i = 0; i< client_number; i++){
            while(clients[i].pong == 0){
                close_client(clients[i].src_addr);
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
}

void * listen_thread_main(void *args){
    if((epoll_fd = epoll_create1(0)) == -1){
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    event.events = EPOLLIN | EPOLLRDHUP;
    init_local();
    init_web();
    struct Msg msg;
    while(1){
        if(epoll_wait(epoll_fd, &event, 1,-1) == -1){
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        if ((event.events & EPOLLERR) != 0 || (event.events & EPOLLHUP) != 0 || (event.events & EPOLLRDHUP) != 0) {
            printf("blad fd\n");
        }
        else {
            struct sockaddr addr;
            socklen_t len = sizeof(addr);
            if (recvfrom(event.data.fd, &msg, sizeof(msg), MSG_WAITALL,&addr,&len) == -1) {
                perror("recv");
                exit(EXIT_FAILURE);
            }
            switch (msg.type) {
                case HELLO_TYPE:
                    add_client(event.data.fd,msg.buf,addr);
                    break;
                case RESULT_TYPE:
                    printf("%s\n",msg.buf);
                    break;
                case KILL_TYPE:
                    close_client(addr);
                    break;
                case PONG_TYPE:
                    pthread_mutex_lock(&clients_mutex);
                    for(int i = 0; i< client_number; i++){
                        if(memcmp(&clients[i].src_addr,&addr,sizeof(addr))==0){
                            clients[i].pong++;
                            break;
                        }
                    }
                    pthread_mutex_unlock(&clients_mutex);
                    break;
            }
        }
    }
}

void assign(struct Operation op){
    if(client_number == 0){
        printf("Brak klientow\n");
        return;
    }
    else{
        pthread_mutex_lock(&clients_mutex);
        int i = rand()%client_number;
        struct Msg msg;
        msg.type = OPERATION_TYPE;
        sprintf(msg.buf,OPERATION_S,op.id,op.a,op.b,op.op);
        sendto(clients[i].fd,&msg,sizeof(msg),0,&clients[i].src_addr,sizeof(clients[i].src_addr));
        pthread_mutex_unlock(&clients_mutex);
    }
}

void stop(){
    while(client_number > 0){
        close_client(clients[0].src_addr);
    }
    close(epoll_fd);
    shutdown(web_sock,SHUT_RDWR);
    shutdown(local_sock,SHUT_RDWR);
    close(local_sock);
    close(web_sock);
    unlink(filename);
    pthread_mutex_destroy(&clients_mutex);
}

void quit(){
    exit(EXIT_SUCCESS);
}

int main(int argv, char ** argc) {
    srand(time(NULL));
    if(argv < 3){
        printf("Zbyt malo argumentow, potrzeba 2\n");
        exit(EXIT_FAILURE);
    }
    port = atoi(argc[1]);
    filename = argc[2];
    signal(SIGINT,quit);
    atexit(stop);

    pthread_t listen_thread;
    if(pthread_create(&listen_thread, NULL, listen_thread_main, NULL)){
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    pthread_t ping_thread;
    if(pthread_create(&ping_thread, NULL, ping_thread_main, NULL)){
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    long nextID = 0;
    while(1){
        printf("$ %ld> ", nextID);
        struct Operation op;
        scanf("%ld %c %ld", &op.a, &op.op, &op.b);
        op.id = nextID++;
        assign(op);
    }
}
