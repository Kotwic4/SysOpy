#include "common.h"

int main(int argv, char ** argc){
    if(argv < 3){
        printf("Za malo argumentów(potrzeba 2)\n");
        exit(EXIT_FAILURE);
    }
    char * filename = argc[1];
    int number_of_record = atoi(argc[2]);
    FILE * handle;
    struct Record* rec = malloc(RECORD_SIZE);
    if((handle = fopen(filename,"w+"))==NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    srand(time(NULL));
    for(int i=1;i<=number_of_record;i++){
        for(int j=0;j<TEXT_SIZE;j++){
            rec->text[j]=0;
        }
        rec->id = i;
        for(int j = 0; j < TEXT_SIZE; j++){
            rec->text[j] = (rand()%('z'-'a'))+'a';
        }
        fwrite(rec, RECORD_SIZE, 1, handle);
    }
    return 0;
}