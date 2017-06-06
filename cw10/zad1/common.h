#ifndef SYSTENY_OPERACYJNE_CW10_COMMON_H
#define SYSTENY_OPERACYJNE_CW10_COMMON_H

#define _BSD_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>



struct Operation{
    long id;
    long a;
    long b;
    char op;
};

#define OPERATION_S "%ld %ld %ld %c"

#define MSG_BUF 1000

#define OPERATION_TYPE 1
#define RESULT_TYPE 2
#define HELLO_TYPE 3
#define KILL_TYPE 4
#define PING_TYPE 5
#define PONG_TYPE 6
#define EXITS_TYPE 7

struct Msg{
    int type;
    char buf[MSG_BUF];
};

#endif //SYSTENY_OPERACYJNE_CW10_COMMON_H