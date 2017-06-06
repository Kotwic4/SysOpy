#include "common.h"
#include <time.h>

struct Node{
    int key;
    mqd_t value;
};
struct Map_node{
    struct Node *value;
    struct Map_node *next;
    struct Map_node *previous;
};
struct Map{
    struct Map_node *head;
    struct Map_node *tail;
};

struct Node* create_node(int key, mqd_t value)
{
    struct Node * node = (struct Node*)malloc(sizeof(struct Node));
    node->key = key;
    node->value = value;
    return node;
}

void print_node(struct Node*a) {
    printf("%d %d\n",a->key, a->value);
}
void free_node(struct Node* a)
{
    free(a);
}

int comp_node(struct Node *a, struct Node *b)
{
    if(a->key == b->key) return 0;
    else return a->key < b->key;
}

int comp_node_value(struct Node *a, int key){
    if(a->key == key) return 0;
    else  return a->key < key;
}

struct Map_node* create_map_node(struct Node *a)
{
    struct Map_node * map_node = (struct Map_node*)malloc(sizeof(struct Map_node));
    map_node->next = NULL;
    map_node->previous = NULL;
    map_node->value = a;
    return map_node;
}

void free_map_node(struct Map_node* a)
{
    free_node(a->value);
    free(a);
}

int comp_map_node(struct Map_node *a, struct Map_node *b)
{
    return comp_node(a->value,b->value);
}

struct Map* create_map()
{
    struct Map * map = (struct Map*)malloc(sizeof(struct Map));
    map->head = create_map_node(NULL);
    map->tail = create_map_node(NULL);
    map->head->next = map->tail;
    map->tail->previous = map->head;
    return map;
}

void free_map(struct Map *a)
{
    struct Map_node* p = a->head->next;
    while(p != a->tail){
        p = p->next;
        free_map_node(p->previous);
    }
    free(a->head);
    free(a->tail);
    free(a);
}

void add_node_map(struct Map *a, struct Node *b)
{
    struct Map_node* node = create_map_node(b);
    node->next = a->head->next;
    a->head->next = node;
    node->next->previous = node;
    node->previous = a->head;
}

void remove_node_map(struct Map *a, struct Node *b)
{
    struct Map_node* p = a->head->next;
    while(p != a->tail){
        if(p->value == b){
            p->next->previous = p->previous;
            p->previous->next = p->next;
            free_map_node(p);
            return;
        }
        p = p->next;
    }
}

struct Node* find_node_map(struct Map *a, int key)
{
    struct Map_node* p = a->head->next;
    while(p != a->tail){
        if(comp_node_value(p->value,key) == 0){
            return p->value;
        }
        p = p->next;
    }
    return NULL;
}

struct Node* pop_node(struct Map *map){
    struct Node* node;
    struct Map_node* p = map->head->next;
    while(p != map->tail){
        p->next->previous = p->previous;
        p->previous->next = p->next;
        node = p->value;
        free(p);
        return node;
    }
    return NULL;
}

mqd_t id;
int next_id = 1;
struct Map * map;
int wait = 1;


void stop(){
    printf("\nStopping Server\n");
    struct Node* node;
    while((node = pop_node(map))!= NULL){
        mq_close(node->value);
        free(node);
    }
    free_map(map);
    if(mq_close(id) < 0 || mq_unlink(SERVERNAME)< 0){
        perror("failed to stop server");
        exit(EXIT_FAILURE);
    }
}

void quit(int sig){
    stop();
    exit(EXIT_SUCCESS);
}



void start(){
    signal(SIGINT,quit);
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 2048;
    id = mq_open(SERVERNAME,O_RDWR| O_CREAT | O_EXCL, 0666, &attr);
    if(id == -1){
        perror("failed to start server");
        exit(EXIT_FAILURE);
    }
    printf("Started Server\n"
                   "Server name is %s\n"
                   "Server id is %d\n",SERVERNAME,id);
}


int main(int argc, char *argv[])
{
    start();
    map = create_map();
    struct msgbuf msg;
    int private_id;
    struct Node* node;
    char buffer[2048];
    struct timespec tm;
    while(1){
        if(wait == 0){
            clock_gettime(CLOCK_REALTIME,&tm);
            if(mq_timedreceive(id, buffer, sizeof (buffer), NULL,&tm) == -1){
                stop();
                exit(EXIT_SUCCESS);
            }
        }
        else{
            if(mq_receive (id, buffer, sizeof (buffer), NULL) == -1){
                stop();
                perror("failed to get msg");
                exit(EXIT_FAILURE);
            }

        }
        sscanf(buffer,"%ld %ld %s\n",&msg.id,&msg.mtype,msg.buf);
        printf("Odebrano nieznane polecenie od %ld typu %ld o tresci \"%s\"\n", msg.id, msg.mtype, msg.buf);
        switch(msg.mtype) {
            case MSG_HELLO:
                private_id = mq_open(msg.buf,O_RDWR);
                msg.id = next_id++;
                add_node_map(map,create_node(msg.id,private_id));
                strcpy(msg.buf,"Odebrano");
                sprintf(buffer,"%ld %ld %s\n",msg.id,msg.mtype,msg.buf);
                mq_send (private_id, buffer, sizeof (buffer), 1);
                break;
            case MSG_ECHO:
                node = find_node_map(map,msg.id);
                if(node != NULL){
                    private_id = node->value;
                    sprintf(buffer,"%ld %ld %s\n",msg.id,msg.mtype,msg.buf);
                    mq_send (private_id, buffer, sizeof (buffer), 1);
                }
                break;
            case MSG_CAPS:
                node = find_node_map(map,msg.id);
                if(node != NULL){
                    private_id = node->value;
                    for(int i = 0; i < BUF_SIZE; i++){
                        if(msg.buf[i] >= 'a' && msg.buf[i] <= 'z'){
                            msg.buf[i] += 'A' - 'a';
                        }
                    }
                    sprintf(buffer,"%ld %ld %s\n",msg.id,msg.mtype,msg.buf);
                    mq_send (private_id, buffer, sizeof (buffer), 1);
                }
                break;
            case MSG_TIME:
                node = find_node_map(map,msg.id);
                if(node != NULL){
                    private_id = node->value;
                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    sprintf(msg.buf,"%d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                    sprintf(buffer,"%ld %ld %s\n",msg.id,msg.mtype,msg.buf);
                    mq_send (private_id, buffer, sizeof (buffer), 1);
                }
                break;
            case MSG_QUIT:
                node = find_node_map(map,msg.id);
                if(node != NULL){
                    wait = 0;
                }
                break;
            case MSG_END:
                node = find_node_map(map,msg.id);
                if(node != NULL){
                    remove_node_map(map,node);
                    mq_close(node->value);
                }
                break;
        }
    }
}
