
#ifndef SYSTENY_OPERACYJNE_CW06_ZAD01_COMMON_H
#define SYSTENY_OPERACYJNE_CW06_ZAD01_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

#define SERVERNAME "/server"

#define MSG_HELLO 1
#define MSG_ECHO 2
#define MSG_CAPS 3
#define MSG_TIME 4
#define MSG_QUIT 5
#define MSG_END 6

#define BUF_SIZE 100


struct msgbuf{
    long mtype;
    long id;
    char buf[BUF_SIZE];
};

#endif //SYSTENY_OPERACYJNE_CW06_ZAD01_COMMON_H
