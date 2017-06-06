#define _GNU_SOURCE
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <dirent.h>
#include <memory.h>

int size;

static int
display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{
    if(tflag == FTW_F && sb->st_size <= size) {
        printf("%s %ld ",
               fpath,(intmax_t) sb->st_size);
        printf( (sb->st_mode & S_IRUSR) ? "r" : "-");
        printf( (sb->st_mode & S_IWUSR) ? "w" : "-");
        printf( (sb->st_mode & S_IXUSR) ? "x" : "-");
        printf( (sb->st_mode & S_IRGRP) ? "r" : "-");
        printf( (sb->st_mode & S_IWGRP) ? "w" : "-");
        printf( (sb->st_mode & S_IXGRP) ? "x" : "-");
        printf( (sb->st_mode & S_IROTH) ? "r" : "-");
        printf( (sb->st_mode & S_IWOTH) ? "w" : "-");
        printf( (sb->st_mode & S_IXOTH) ? "x" : "-");
        time_t t = sb->st_mtime;
        struct tm lt;
        localtime_r(&t, &lt);
        char timbuf[80];
        strftime(timbuf, sizeof(timbuf), "%c", &lt);
        printf(" %s\n", timbuf);
    }
    if(tflag == FTW_D && ftwbuf->level != 0){
        return FTW_SKIP_SUBTREE;
    }
    return FTW_CONTINUE;
}

int nftw_ls(char * filename){

    int flags;
    flags = FTW_ACTIONRETVAL | FTW_PHYS;
    if (nftw(filename, display_info, 20, flags) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int sys_ls(char * filename){

    DIR *mydir;
    struct dirent *myfile;
    struct stat sb;
    mydir = opendir(filename);
    char buf[512];
    while((myfile = readdir(mydir)) != NULL)
    {
        sprintf(buf, "%s/%s",filename, myfile->d_name);
        stat(buf, &sb);
        if(S_ISREG(sb.st_mode) && sb.st_size <= size) {
            printf("%s %ld ",
                   buf,(intmax_t) sb.st_size);
            printf( (sb.st_mode & S_IRUSR) ? "r" : "-");
            printf( (sb.st_mode & S_IWUSR) ? "w" : "-");
            printf( (sb.st_mode & S_IXUSR) ? "x" : "-");
            printf( (sb.st_mode & S_IRGRP) ? "r" : "-");
            printf( (sb.st_mode & S_IWGRP) ? "w" : "-");
            printf( (sb.st_mode & S_IXGRP) ? "x" : "-");
            printf( (sb.st_mode & S_IROTH) ? "r" : "-");
            printf( (sb.st_mode & S_IWOTH) ? "w" : "-");
            printf( (sb.st_mode & S_IXOTH) ? "x" : "-");
            time_t t = sb.st_mtime;
            struct tm lt;
            localtime_r(&t, &lt);
            char timbuf[80];
            strftime(timbuf, sizeof(timbuf), "%c", &lt);
            printf(" %s\n", timbuf);
        }
    }
    closedir(mydir);
    return 0;
}

int main(int argc, char *argv[])
{

    if(argc < 3){
        printf("nalezy podac przynajmniej 2 argumenty\n");
        return 1;
    }

    char* filename = realpath(argv[1],NULL);
    size = atoi(argv[2]);

    int result;
    if(argc < 4 || strcmp(argv[3],"nftw") == 0){
        result = sys_ls(filename);
    }
    else{
        result = nftw_ls(filename);
    }

    free (filename);

    return result;
}