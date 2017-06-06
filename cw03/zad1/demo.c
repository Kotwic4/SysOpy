#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Brak 1 argumentu(nazwa zmienej)!\n");
        return 1;
    }
    char* value = getenv(argv[1]);
    if(value == NULL){
        printf("Brak ustawionej zmienej %s\n", argv[1]);
    }
    else{
        printf("Zmiena %s ustawiona na wartosc %s\n",argv[1],value);
    }
    return 0;
}
