#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>


enum Op_type{
    generate_type,
    shuffle_type,
    sort_type
};

enum In_type{
    sys,
    lib
};

struct Input_set{
    enum Op_type op_type;
    enum In_type in_type;
    char * filename;
    int records_number;
    int records_width;
};

int sort(char *filename, int number, int width, enum In_type type);

int shuffle(char *filename, int number, int width, enum In_type type);

int parse_input(struct Input_set* input_set, int argc, char*argv[]){
    int i = 1;
    if(argc == i){
        printf("Brak %d argumentu!(typ operacji)\n",i);
        return 1;
    }
    else{
        if(strcmp(argv[i],"generate") == 0){
            input_set->op_type = generate_type;
        }
        else if(strcmp(argv[i],"shuffle") == 0){
            input_set->op_type = shuffle_type;
        }
        else if(strcmp(argv[i],"sort") == 0){
            input_set->op_type = sort_type;
        }
        else{
            printf(" %d argumentem powinien byc typ operacji generate/shuffle/sort\n", i);
            return 1;
        }
        i++;
    }
    if(input_set->op_type != generate_type){
        if(argc == i){
            printf("Brak %d argumentu!(typ funkcji)\n",i);
            return 1;
        }
        if(strcmp(argv[i],"sys") == 0){
            input_set->in_type = sys;
        }
        else if(strcmp(argv[i],"lib") == 0){
            input_set->in_type = lib;
        }
        else{
            printf("%d argumentem powinien byc typ funkcji sys/lib\n",i);
            return 1;
        }
        i++;
    }
    if(argc == i){
        printf("Brak %d argumentu(sciezka do pliku)!\n",i);
        return 1;
    }
    else{
        input_set->filename = argv[i];
        i++;
    }
    if(argc == i){
        printf("Brak %d argumentu(wielkosc rekordu)!\n",i);
        return 1;
    }
    else{
        input_set->records_width = atoi(argv[i]);
        if(input_set->records_width <= 0){
            printf("Wartosc %d argumentu musi byc wieksza od zera\n",i);
            return 1;
        }
        i++;
    }
    if(argc == i){
        printf("Brak %d argumentu(ilosc rekordow)!\n",i);
        return 1;
    }
    else{
        input_set->records_number = atoi(argv[i]);
        if(input_set->records_number <= 0){
            printf("Wartosc %d argumentu musi byc wieksza od zera\n",i);
            return 1;
        }
        i++;
    }
    return 0;
}

int generate(char * filename,int records_number,int records_width){
    int randomData = open("/dev/urandom", O_RDONLY);
    if(randomData == -1){
        perror("/dev/random");
        return 1;
    }
    int out = open(filename,O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
    if(out == -1){
        close(randomData);
        perror(filename);
        return 1;
    }
    char *myRandomData = calloc(records_width,sizeof (char));
    for(int i = 0; i < records_number; i++){
        ssize_t result = read(randomData, myRandomData, records_width);
        if (result < 0)
        {
            close(randomData);
            close(out);
            free(myRandomData);
            perror("/dev/random");
            return 1;
        }
        result = write(out,myRandomData,records_width);
        if (result < 0)
        {
            close(randomData);
            close(out);
            free(myRandomData);
            perror(filename);
            return 1;
        }
    }
    close(randomData);
    close(out);
    free(myRandomData);
    return 0;
}

int sys_sort(char * filename,int records_number,int records_width) {
    int file = open(filename,O_RDWR);
    if(file == -1){
        perror(filename);
        return 1;
    }
    unsigned char *row_a = calloc(records_width,sizeof (unsigned char));
    unsigned char *row_b = calloc(records_width,sizeof (unsigned char));

    int n = records_number;
    do{
        for(int i = 0; i<n-1;i++){
            lseek(file,i*records_width,SEEK_SET);
            ssize_t result = read(file, row_a, records_width);
            if (result < 0)
            {
                perror(filename);
                close(file);
                free(row_a);
                free(row_b);
                return 1;
            }
            result = read(file, row_b, records_width);
            if (result < 0)
            {
                perror(filename);
                close(file);
                free(row_a);
                free(row_b);
                return 1;
            }
            if(row_a[0] > row_b[0]){
                lseek(file,i*records_width,SEEK_SET);
                result = write(file, row_b, records_width);
                if (result < 0)
                {
                    perror(filename);
                    close(file);
                    free(row_a);
                    free(row_b);
                    return 1;
                }
                result = write(file, row_a, records_width);
                if (result < 0)
                {
                    perror(filename);
                    close(file);
                    free(row_a);
                    free(row_b);
                    return 1;
                }
            }
        }
        n--;
    } while (n > 1);
    close(file);
    free(row_a);
    free(row_b);
    return 0;
}

int sys_shuffle(char * filename,int records_number,int records_width){
    time_t tt;
    srand((unsigned int) time(&tt));
    int file = open(filename,O_RDWR);
    if(file == -1){
        perror(filename);
        return 1;
    }
    unsigned char *row_a = calloc(records_width,sizeof (unsigned char));
    unsigned char *row_b = calloc(records_width,sizeof (unsigned char));
    for(int i = 0; i<records_number-1;i++){
        int j = rand()%(records_number-i)+i;
        if(i!=j){
            lseek(file,i*records_width,SEEK_SET);
            ssize_t result = read(file, row_a, records_width);
            if (result < 0)
            {
                perror(filename);
                close(file);
                free(row_a);
                free(row_b);
                return 1;
            }
            lseek(file,j*records_width,SEEK_SET);
            result = read(file, row_b, records_width);
            if (result < 0)
            {
                perror(filename);
                close(file);
                free(row_a);
                free(row_b);
                return 1;
            }
            lseek(file,i*records_width,SEEK_SET);
            result = write(file, row_b, records_width);
            if (result < 0)
            {
                perror(filename);
                close(file);
                free(row_a);
                free(row_b);
                return 1;
            }
            lseek(file,j*records_width,SEEK_SET);
            result = write(file, row_a, records_width);
            if (result < 0)
            {
                perror(filename);
                close(file);
                free(row_a);
                free(row_b);
                return 1;
            }
        }
    }
    close(file);
    free(row_a);
    free(row_b);
    return 0;
}

int lib_sort(char * filename,int records_number,int records_width) {
    FILE* file = fopen(filename,"r+");
    if(file == NULL){
        perror(filename);
        return 1;
    }
    unsigned char *row_a = calloc(records_width,sizeof (unsigned char));
    unsigned char *row_b = calloc(records_width,sizeof (unsigned char));

    int n = records_number;
    do{
        for(int i = 0; i<n-1;i++){
            fseek(file,i*records_width,SEEK_SET);
            ssize_t result = fread(row_a,1,records_width,file);
            if (result < 0)
            {
                perror(filename);
                fclose(file);
                free(row_a);
                free(row_b);
                return 1;
            }
            result = fread(row_b,1,records_width,file);
            if (result < 0)
            {
                perror(filename);
                fclose(file);
                free(row_a);
                free(row_b);
                return 1;
            }
            if(row_a[0] > row_b[0]){
                fseek(file,i*records_width,SEEK_SET);
                result = fwrite(row_b,1,records_width,file);
                if (result < 0)
                {
                    perror(filename);
                    fclose(file);
                    free(row_a);
                    free(row_b);
                    return 1;
                }
                result = fwrite(row_a,1,records_width,file);
                if (result < 0)
                {
                    perror(filename);
                    fclose(file);
                    free(row_a);
                    free(row_b);
                    return 1;
                }
                fflush(file);
            }
        }
        n--;
    } while (n > 1);
    fclose(file);
    free(row_a);
    free(row_b);
    return 0;
}

int lib_shuffle(char * filename,int records_number,int records_width){
    time_t tt;
    srand((unsigned int) time(&tt));
    FILE* file = fopen(filename,"r+");
    if(file == NULL){
        perror(filename);
        return 1;
    }
    unsigned char *row_a = calloc(records_width,sizeof (unsigned char));
    unsigned char *row_b = calloc(records_width,sizeof (unsigned char));
    for(int i = 0; i<records_number-1;i++){
        int j = rand()%(records_number-i)+i;
        if(i!=j){
            fseek(file,i*records_width,SEEK_SET);
            ssize_t result = fread(row_a,1,records_width,file);
            if (result < 0)
            {
                perror(filename);
                fclose(file);
                free(row_a);
                free(row_b);
                return 1;
            }
            fseek(file,j*records_width,SEEK_SET);
            result = fread(row_b,1,records_width,file);
            if (result < 0)
            {
                perror(filename);
                fclose(file);
                free(row_a);
                free(row_b);
                return 1;
            }
            fseek(file,i*records_width,SEEK_SET);
            result = fwrite(row_b,1,records_width,file);
            if (result < 0)
            {
                perror(filename);
                fclose(file);
                free(row_a);
                free(row_b);
                return 1;
            }
            fseek(file,j*records_width,SEEK_SET);
            result = fwrite(row_a,1,records_width,file);
            if (result < 0)
            {
                perror(filename);
                fclose(file);
                free(row_a);
                free(row_b);
                return 1;
            }
            fflush(file);
        }
    }
    fclose(file);
    free(row_a);
    free(row_b);
    return 0;
}

int shuffle(char *filename, int number, int width, enum In_type type) {
    if(type == sys) return sys_shuffle(filename,number,width);
    else return lib_shuffle(filename,number,width);
}

int sort(char *filename, int number, int width, enum In_type type) {
    if(type == sys) return sys_sort(filename,number,width);
    else return lib_sort(filename,number,width);
}

int main(int argc,char * argv[]){

    struct Input_set* input_set = malloc(sizeof(struct Input_set));
    int result = parse_input(input_set,argc,argv);
    if(result == 0){
        switch (input_set->op_type){
            case generate_type:
                result = generate(input_set->filename,input_set->records_number,input_set->records_width);
                break;
            case sort_type:
                result = sort(input_set->filename,input_set->records_number,input_set->records_width,input_set->in_type);
                break;
            case shuffle_type:
                result = shuffle(input_set->filename,input_set->records_number,input_set->records_width,input_set->in_type);
                break;
        }
    }
    free(input_set);
    return result;
}