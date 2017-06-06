#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
    if(argc != 4){
        printf("za malo argumentow\n");
        exit(EXIT_FAILURE);
    }
    char *fn=argv[1];
    int N = atoi(argv[2]);
    int K = atoi(argv[3]);
    srand(getpid());
    FILE *wfd;
    wfd = fopen(fn, "a");
    for(int i = 0; i < N ; i++){
        double re_0 = (double)rand()/RAND_MAX*3.0-2.0; //float in range -2 to 1
        double im_0 = (double)rand()/RAND_MAX*2.0-1.0; //float in range -1 to 1
        double re = 0.0;
        double im = 0.0;
        int val = 0;
        while ((re*re + im*im < 2*2)  &&  val < K) {
            double re_temp = re*re - im*im + re_0;
            im = 2*re*im + im_0;
            re = re_temp;
            val++;
        }
        fprintf(wfd,"%lf %lf %d\n",re_0,im_0,val);
        fflush(wfd);
    }
    return 0;
}