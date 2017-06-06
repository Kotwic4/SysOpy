
#ifndef SYSTENY_OPERACYJNE_CW07_ZAD02_COMMON_H
#define SYSTENY_OPERACYJNE_CW07_ZAD02_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


#define SERVERNAME "/server"
#define FIFO_NAME "/fifo"
#define FIFO_SIZE sizeof(pid_t)*max[0]
#define NUM_NAME "/num"
#define NUM_SIZE sizeof(int)
#define MAX_NAME "/max"
#define MAX_SIZE sizeof(int)

/*
 * 0 - barber status(1-active,0-sleep)
 * 1 - semafor for change(1-free,0-taken)
 * 2 - queue(1-next client,0-client have to wait)
 * 3 - barbering(1-finish,0-working)
 * 4 - char(1-taken,0-free)
 */
#define SBARBER_NAME "/SBARBER"
#define SFIFO_NAME "/SFIFO"
#define SQUEUE_NAME "/SQUEUE"
#define SWORK_NAME "/SWORK"
#define SCHAIR_NAME "SCHAIR"


#endif //SYSTENY_OPERACYJNE_CW07_ZAD02_COMMON_H
