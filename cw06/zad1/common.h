
#ifndef SYSTENY_OPERACYJNE_CW06_ZAD01_COMMON_H
#define SYSTENY_OPERACYJNE_CW06_ZAD01_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define HOME getenv("HOME")
#define SERVER_KEY ftok(HOME,'6')

#define MSG_HELLO 1
#define MSG_ECHO 2
#define MSG_CAPS 3
#define MSG_TIME 4
#define MSG_QUIT 5

#define BUF_SIZE 50

struct msgbuf{
    long mtype;
    long id;
    char buf[BUF_SIZE];
};

#define MSG_SIZE (sizeof(struct msgbuf)-sizeof(long))

#endif //SYSTENY_OPERACYJNE_CW06_ZAD01_COMMON_H
