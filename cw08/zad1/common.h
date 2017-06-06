#ifndef CW08_COMMON_H
#define CW08_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define RECORD_SIZE 1024
#define TEXT_SIZE 1024-sizeof(int)

struct Record{
    int id;
    char text[TEXT_SIZE];
};

#endif //CW08_COMMON_H
