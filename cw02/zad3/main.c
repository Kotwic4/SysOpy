#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>


struct flock* get_lock(int file,long long int a){
    struct flock *lock = malloc(sizeof(struct flock));
    lock->l_type=F_WRLCK;
    lock->l_len=1;
    lock->l_start=a;
    lock->l_whence=SEEK_SET;
    fcntl(file,F_GETLK,lock);
    return lock;
}

int read_lock(int file,int wait,long long int a){

    struct flock lock;
    lock.l_type=F_RDLCK;
    lock.l_len=1;
    lock.l_start=a;
    lock.l_whence=SEEK_SET;
    int result;
    if(wait == 0){
        result = fcntl(file,F_SETLK,&lock);
        if(result == -1){
            printf("blad nalozenia blokady\n");
            return -1;
        }
        else{
            printf("nalozono blokade\n");
        }
    }else
    {
        fcntl(file,F_SETLKW,&lock);
    }
    return 0;

}

int write_lock(int file, int wait,long long int a){
    struct flock lock;
    lock.l_type=F_WRLCK;
    lock.l_len=1;
    lock.l_start=a;
    lock.l_whence=SEEK_SET;
    int result;
    if(wait == 0){
        result = fcntl(file,F_SETLK,&lock);
        if(result == -1){
            printf("blad nalozenia blokady\n");
            return -1;
        }
        else{
            printf("nalozono blokade\n");
        }
    }else
    {
        fcntl(file,F_SETLKW,&lock);
    }
    return 0;
}

void list_lock(int file){
    long long int end = lseek(file,0,SEEK_END);
    struct flock *lock;
    for(long long int i = 0; i <= end; i++){
        lock = get_lock(file,i);
        if(lock->l_type != F_UNLCK){
            printf("%lld %d", i, lock->l_pid);
            if(lock->l_type == F_WRLCK) printf(" W");
            else printf(" R");
        }
    }
}

int remove_lock(int file,long long int a){
    struct flock lock;
    lock.l_type=F_UNLCK;
    lock.l_len=1;
    lock.l_start=a;
    lock.l_whence=SEEK_SET;
    int result;
    result = fcntl(file,F_SETLK,&lock);
    if(result == -1){
        printf("blad zdjecia blokady\n");
        return -1;
    }
    else{
        printf("zdjeto blokade\n");
    }
    return 0;
}

void read_byte(int file,long long int a){
    if(read_lock(file,0,a)==-1) return;
    char buf[1];
    lseek(file,a,SEEK_SET);
    read(file,buf,1);
    printf("%s\n",buf);
    remove_lock(file,a);
}

void write_byte(int file,long long int a,char c){
    char buf[1];
    buf[0] = c;
    if(write_lock(file,0,a)==-1) return;
    lseek(file,a,SEEK_SET);
    write(file,buf,1);
    remove_lock(file,a);
    return;
}

int main(int argc, char *argv[]){
    int file = open(argv[1],O_RDWR);
    if(file == -1){
        perror(argv[1]);
        return 1;
    }
    long long int a;
    char c;
    while(1){
        char buf[100];
        printf("\n>");
        scanf("%s",buf);
        if(strcmp(buf,":q")==0){
            close(file);
            return 0;
        }
        if(strcmp(buf,"read_lock")==0){
            scanf("%lld",&a);
            read_lock(file,0,a);
            continue;
        }
        if(strcmp(buf,"read_lock_force")==0){
            scanf("%lld",&a);
            read_lock(file,1,a);
            continue;
        }
        if(strcmp(buf,"write_lock")==0){
            scanf("%lld",&a);
            write_lock(file,0,a);
            continue;
        }
        if(strcmp(buf,"write_lock_force")==0){
            scanf("%lld",&a);
            write_lock(file,1,a);
            continue;
        }
        if(strcmp(buf,"list_lock")==0){
            list_lock(file);
            continue;
        }
        if(strcmp(buf,"remove_lock")==0){
            scanf("%lld",&a);
            remove_lock(file,a);
            continue;
        }
        if(strcmp(buf,"read_byte")==0){
            scanf("%lld",&a);
            read_byte(file,a);
            continue;
        }
        if(strcmp(buf,"write_byte")==0){
            scanf("%lld %c",&a,&c);
            write_byte(file,a,c);
            continue;
        }
    }
}