#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


double getX(int i, int R){
    return ((double)i*3.0/(double)R)-2.0;
}

double getY(int j, int R){
    return ((double)j*2.0/(double)R)-1.0;
}

int main(int argc, char *argv[])
{
    if(argc != 3){
        printf("za malo argumentow\n");
        exit(EXIT_FAILURE);
    }
    char *fn=argv[1];
    int R = atoi(argv[2]);
    int **t = calloc(R,sizeof(int*));
    for(int i = 0; i < R; i++){
        t[i] = calloc(R,sizeof(int));
        for(int j = 0; j < R; j++){
            t[i][j] = 0;
        }
    }
    FILE * rfd;
    if( access( fn, F_OK ) != -1 ) {
        remove(fn);
    }
    if(mkfifo(fn, 0777) < 0){
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    rfd = fopen(fn,"r");
    double re,im;
    int val;
    while(fscanf(rfd,"%lf %lf %d\n",&re,&im,&val) != EOF){
        int i,j;
        for(i = 0; i < R; i++){
            if(getX(i,R) > re){
                break;
            }
        }
        i--;
        for(j = 0; j < R; j++){
            if(getY(j,R)> im){
                break;
            }
        }
        j--;
        t[i][j] = val;
    }
    fclose(rfd);
    remove(fn);
    FILE * wfd = fopen("data","w");
    for(int i = 0; i < R; i++){
        for (int j = 0; j < R; j++){
            fprintf(wfd,"%d %d %d\n",i,j,t[i][j]);
        }
    }
    fclose(wfd);
    FILE* gnuplot = popen("gnuplot -persistent", "w");
    fprintf(gnuplot,"set view map\n");
    fprintf(gnuplot,"set xrange [0:%d]\n",R);
    fprintf(gnuplot,"set yrange [0:%d]\n",R);
    fprintf(gnuplot,"plot 'data' with image\n");
    fflush(gnuplot);
    getc(stdin);
    return 0;
}