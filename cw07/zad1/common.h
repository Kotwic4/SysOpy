
#ifndef SYSTENY_OPERACYJNE_CW07_ZAD01_COMMON_H
#define SYSTENY_OPERACYJNE_CW07_ZAD01_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define HOME getenv("HOME")
#define FIFO_KEY ftok(HOME,'f')
#define NUM_KEY ftok(HOME,'n')
#define MAX_KEY ftok(HOME,'m')
#define SERVER_KEY ftok(HOME,'7')
/*
 * 0 - barber status(0-active,1-sleep)
 * 1 - semafor for change(0-free,1-taken)
 * 2 - queue(0-next client,1-client have to wait)
 * 3 - barbering(0-finish,1-working)
 * 4 - char(0-taken,1-free)
 */
#define SEMAFOR_NUM 5
#define SBARBER 0
#define SFIFO 1
#define SQUEUE 2
#define SWORK 3
#define SCHAIR 4


#endif //SYSTENY_OPERACYJNE_CW07_ZAD01_COMMON_H
