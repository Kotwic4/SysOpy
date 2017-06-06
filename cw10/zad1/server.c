#include "common.h"

#include <pthread.h>
#include <sys/epoll.h>

#define N 100
#define PING_SLEEP 5

struct Client{
    int fd;
    char name[MSG_BUF];
    int pong;
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
    web_sock = socket(AF_INET,SOCK_STREAM,0);
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
    if(listen(web_sock,N) == -1){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    event.data.fd = web_sock;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, web_sock, &event)==-1){
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
}

void init_local(){
    local_sock = socket(AF_UNIX,SOCK_STREAM,0);
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
    if(listen(local_sock,N) == -1){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    event.data.fd = local_sock;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, local_sock, &event)==-1){
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
}

void add_client(int fd, char*name){
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
        send(fd,&msg,sizeof(msg),0);
        close(fd);
    }
    else{
        clients[client_number].fd = fd;
        clients[client_number].pong = -1;
        strcpy(clients[client_number].name,name);
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
            perror("epoll_ctl");
            exit(EXIT_FAILURE);
        }
        client_number++;
    }
    pthread_mutex_unlock(&clients_mutex);
}

void close_client(int fd){
    pthread_mutex_lock(&clients_mutex);
    for(int i = 0 ; i < client_number; i++){
        if(clients[i].fd == fd){
            for(int j = i+1; j < client_number; j++){
                i = j-1;
                clients[i].fd = clients[j].fd;
                clients[i].pong = clients[j].pong;
                strcpy(clients[i].name,clients[j].name);
            }
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = fd;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev) == -1) {
                perror("epoll_ctl");
                exit(EXIT_FAILURE);
            }
            client_number--;
            close(fd);
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
            send(clients[i].fd,&msg,sizeof(msg),0);
        }
        pthread_mutex_unlock(&clients_mutex);
        sleep(PING_SLEEP);
        pthread_mutex_lock(&clients_mutex);
        for(int i = 0; i< client_number; i++){
            while(clients[i].pong == 0){
                close_client(clients[i].fd);
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
            if (event.data.fd != local_sock && event.data.fd != web_sock) {
                close_client(event.data.fd);
            }
        }else if (event.data.fd == local_sock || event.data.fd == web_sock) {
                struct sockaddr in_addr;
                socklen_t in_len = sizeof(in_addr);
                int fd = accept(event.data.fd, &in_addr, &in_len);
                if (fd == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                recv(fd, &msg, sizeof(msg), MSG_WAITALL);
                if(msg.type == HELLO_TYPE){
                    add_client(fd,msg.buf);
                } else{
                    close(fd);
                }

        } else {
            if (recv(event.data.fd, &msg, sizeof(msg), MSG_WAITALL) == -1) {
                perror("recv");
                exit(EXIT_FAILURE);
            }
            switch (msg.type) {
                case RESULT_TYPE:
                    printf("%s\n",msg.buf);
                    break;
                case KILL_TYPE:
                    close_client(event.data.fd);
                    break;
                case PONG_TYPE:
                    pthread_mutex_lock(&clients_mutex);
                    for(int i = 0; i< client_number; i++){
                        if(clients[i].fd == event.data.fd){
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
        write(clients[i].fd,&msg,sizeof(msg));
        pthread_mutex_unlock(&clients_mutex);
    }
}

void stop(){
    while(client_number > 0){
        close_client(clients[0].fd);
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
