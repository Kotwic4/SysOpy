#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include "../zad1/book.h"
#include <dlfcn.h>

#define CLOCKS_PER_SEC_TMS 100.0


struct person{
    char first_name[80],last_name[80],birth_date[80],email[80],phone[80],address[80];
};


static const int INPUT_LENGHT = 1000;



void get_data(struct person tab[]){
    for(int i = 0; i<INPUT_LENGHT;i++){
        scanf("%s %s %s %s %s %s",tab[i].first_name,tab[i].last_name,tab[i].birth_date,tab[i].email,tab[i].phone,tab[i].address);
    }
}

void print_time(clock_t startTime,clock_t stopTime, struct tms a, struct tms b,long double modifier){

    long double real,user,sys;
    real = (long double)(stopTime - startTime) / (long double)CLOCKS_PER_SEC/modifier;
    user = (long double)(b.tms_utime - a.tms_utime) / (long double)CLOCKS_PER_SEC_TMS/modifier;
    sys = (long double)(b.tms_utime - a.tms_utime) / (long double)CLOCKS_PER_SEC_TMS/modifier;
    printf("real:%LF user:%LF sys:%LF\n", real,user,sys);
}

void test_list(struct person tab[]){

    printf("\ntesting list\n");
    clock_t startTime,stopTime;
    struct tms a,b;

    //*************************************
    //create - start time
    printf("\ntesting create\n");
    startTime = clock();
    times(&a);

    //create - do
#ifndef DYNLAB
    struct List_book* list = create_list_book();
#else
    void *handle;
    struct List_book*  (*create_list_book)();
    handle = dlopen("../zad1/libbook.so", RTLD_LAZY);
    create_list_book = (struct List_book*  (*)()) dlsym(handle, "create_list_book");
    struct List_book* list = (*create_list_book)();
#endif


    //create - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //create -end
    //**************************************


    //**************************************
    //add elements - start time
    printf("\ntesting add(average time)\n");
    startTime = clock();
    times(&a);

    //add elements - do
    for(int i = 0; i<INPUT_LENGHT;i++){
#ifndef DYNLAB
        struct Date* date = convert_date(tab[i].birth_date);
        struct Node* node = create_node(tab[i].first_name,tab[i].last_name,date,tab[i].email,tab[i].phone,tab[i].address);
        add_node_list_book(list,node);
#else
        struct Date* (*convert_date)(char*);
        convert_date = (struct Date*(*)(char*)) dlsym(handle, "convert_date");
        struct Date* date = (*convert_date)(tab[i].birth_date);

        struct Node * (*create_node)(char*,char*,struct Date*,char*,char*,char*);
        create_node = (struct Node *(*)(char*,char*,struct Date*,char*,char*,char*)) dlsym(handle, "create_node");
        struct Node * node = (*create_node)(tab[i].first_name,tab[i].last_name,date,tab[i].email,tab[i].phone,tab[i].address);

        void (*add_node_list_book)(struct List_book*,struct Node *);
        add_node_list_book = (void (*)(struct List_book*,struct Node *)) dlsym(handle, "add_node_list_book");
        (*add_node_list_book)(list,node);
#endif

    }

    //add elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,INPUT_LENGHT);
    //add elements -end
    //**************************************


    //**************************************
    //find elements - start time
    printf("\ntesting find(optimistic time)\n");
    startTime = clock();
    times(&a);

    //find elements - do
#ifndef DYNLAB
    struct Node * o = find_node_list_book(list,tab[INPUT_LENGHT-1].phone,PHONE);
#else
    struct Node * (*find_node_list_book)(struct List_book*,char*,enum comp_node_enum);
    find_node_list_book = (struct Node *(*)(struct List_book*,char*,enum comp_node_enum)) dlsym(handle, "find_node_list_book");
    struct Node * o = (*find_node_list_book)(list,tab[INPUT_LENGHT-1].phone,PHONE);
#endif

    //find elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //find elements -end
    //**************************************


    //**************************************
    //find elements - start time
    printf("\ntesting find(pesimistic time)\n");
    startTime = clock();
    times(&a);

    //find elements - do
#ifndef DYNLAB
    struct Node * p = find_node_list_book(list,tab[0].phone,PHONE);
#else
    struct Node * p = (*find_node_list_book)(list,tab[0].phone,PHONE);
#endif

    //find elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //find elements -end
    //**************************************


    //**************************************
    //delete elements - start time
    printf("\ndelete delete(optimistic time)\n");
    startTime = clock();
    times(&a);

    //delete elements - do
#ifndef DYNLAB
    remove_node_list_book(list,o);
#else
    void  (*remove_node_list_book)(struct List_book*,struct Node*);
    remove_node_list_book = (void (*)(struct List_book*,struct Node*)) dlsym(handle, "remove_node_list_book");
    (*remove_node_list_book)(list,o);
#endif

    //delete elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //delete elements -end
    //**************************************


    //**************************************
    //delete elements - start time
    printf("\ntesting delete(pesimistic time)\n");
    startTime = clock();
    times(&a);

    //delete elements - do
#ifndef DYNLAB
    remove_node_list_book(list,p);
#else
    (*remove_node_list_book)(list,p);
#endif

    //delete elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //delete elements -end
    //**************************************

    //**************************************
    //sort elements - start time
    printf("\ntesting sort\n");
    startTime = clock();
    times(&a);

    //sort elements - do
#ifndef DYNLAB
    sort_list_book(list,BIRTH_DATE);
#else
    void  (*sort_list_book)(struct List_book*,enum comp_node_enum);
    sort_list_book = (void (*)(struct List_book*,enum comp_node_enum)) dlsym(handle, "sort_list_book");
    (*sort_list_book)(list,BIRTH_DATE);
#endif

    //sort elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //sort elements -end
    //**************************************


    //**************************************
    //free elements - start time
    printf("\ntesting free\n");
    startTime = clock();
    times(&a);

    //free elements - do

#ifndef DYNLAB
    free_list_book(list);
#else
    void  (*free_list_book)(struct List_book*);
    free_list_book = (void (*)(struct List_book*)) dlsym(handle, "free_list_book");
    (*free_list_book)(list);
    dlclose(handle);
#endif

    //free elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //free elements -end
    //**************************************

}

void test_tree(struct person tab[]){


    printf("\ntesting tree\n");
    clock_t startTime,stopTime;
    struct tms a,b;

    //*************************************
    //create - start time
    printf("\ntesting create\n");
    startTime = clock();
    times(&a);

    //create - do
#ifndef DYNLAB
    struct Tree_book* tree = create_tree_book(PHONE);
#else
    void *handle;
    struct Tree_book*  (*create_tree_book)(enum comp_node_enum);
    handle = dlopen("../zad1/libbook.so", RTLD_LAZY);
    create_tree_book = (struct Tree_book*  (*)(enum comp_node_enum)) dlsym(handle, "create_tree_book");
    struct Tree_book* tree = (*create_tree_book)(PHONE);
#endif

    //create - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //create -end
    //**************************************

    //**************************************
    //add elements - start time
    printf("\ntesting add(average time)\n");
    startTime = clock();
    times(&a);

    //add elements - do
    for(int i = 0; i<INPUT_LENGHT;i++){

#ifndef DYNLAB
        struct Date* date = convert_date(tab[i].birth_date);
        struct Node* node = create_node(tab[i].first_name,tab[i].last_name,date,tab[i].email,tab[i].phone,tab[i].address);
        add_node_tree_book(tree,node);
#else
        struct Date* (*convert_date)(char*);
        convert_date = (struct Date*(*)(char*)) dlsym(handle, "convert_date");
        struct Date* date = (*convert_date)(tab[i].birth_date);

        struct Node * (*create_node)(char*,char*,struct Date*,char*,char*,char*);
        create_node = (struct Node *(*)(char*,char*,struct Date*,char*,char*,char*)) dlsym(handle, "create_node");
        struct Node * node = (*create_node)(tab[i].first_name,tab[i].last_name,date,tab[i].email,tab[i].phone,tab[i].address);

        void (*add_node_tree_book)(struct Tree_book*,struct Node *);
        add_node_tree_book = (void (*)(struct Tree_book*,struct Node *)) dlsym(handle, "add_node_tree_book");
        (*add_node_tree_book)(tree,node);
#endif
    }

    //add elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,INPUT_LENGHT);
    //add elements -end
    //**************************************


    //**************************************
    //find elements - start time
    printf("\ntesting find(optimistic time)\n");
    startTime = clock();
    times(&a);

    //find elements - do
#ifndef DYNLAB
    struct Node * o = find_node_tree_book(tree,tab[INPUT_LENGHT-1].phone);
#else
    struct Node * (*find_node_tree_book)(struct Tree_book*,char*);
    find_node_tree_book = (struct Node *(*)(struct Tree_book*,char*)) dlsym(handle, "find_node_tree_book");
    struct Node * o = (*find_node_tree_book)(tree,tab[INPUT_LENGHT-1].phone);
#endif


    //find elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //find elements -end
    //**************************************


    //**************************************
    //find elements - start time
    printf("\ntesting find(pesimistic time)\n");
    startTime = clock();
    times(&a);

    //find elements - do
#ifndef DYNLAB
    struct Node * p = find_node_tree_book(tree,tab[INPUT_LENGHT-1].phone);
#else
    struct Node * p = (*find_node_tree_book)(tree,tab[INPUT_LENGHT-1].phone);
#endif

    //find elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //find elements -end
    //**************************************

    //**************************************
    //delete elements - start time
    printf("\ndelete delete(optimistic time)\n");
    startTime = clock();
    times(&a);

    //delete elements - do
#ifndef DYNLAB
    remove_node_tree_book(tree,o);
#else
    void  (*remove_node_tree_book)(struct Tree_book*,struct Node*);
    remove_node_tree_book = (void (*)(struct Tree_book*,struct Node*)) dlsym(handle, "remove_node_tree_book");
    (*remove_node_tree_book)(tree,o);
#endif

    //delete elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //delete elements -end
    //**************************************

    //**************************************
    //delete elements - start time
    printf("\ntesting delete(pesimistic time)\n");
    startTime = clock();
    times(&a);

    //delete elements - do
#ifndef DYNLAB
    remove_node_tree_book(tree,p);
#else
    (*remove_node_tree_book)(tree,p);
#endif

    //delete elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //delete elements -end
    //**************************************


    //**************************************
    //rebuild elements - start time
    printf("\ntesting rebuild\n");
    startTime = clock();
    times(&a);

    //rebuild elements - do
#ifndef DYNLAB
    rebuild_tree_book(tree,BIRTH_DATE);
#else
    void  (*rebuild_tree_book)(struct Tree_book*,enum comp_node_enum);
    rebuild_tree_book = (void (*)(struct Tree_book*,enum comp_node_enum)) dlsym(handle, "rebuild_tree_book");
    (*rebuild_tree_book)(tree,BIRTH_DATE);
#endif

    //rebuild elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //rebuild elements -end
    //**************************************

    //**************************************
    //free elements - start time
    printf("\ntesting free\n");
    startTime = clock();
    times(&a);

    //free elements - do
#ifndef DYNLAB
    free_tree_book(tree);
#else
    void  (*free_tree_book)(struct Tree_book*);
    free_tree_book = (void (*)(struct Tree_book*)) dlsym(handle, "free_tree_book");
    (*free_tree_book)(tree);
    dlclose(handle);
#endif

    //free elements - stop
    stopTime = clock();
    times(&b);
    print_time(startTime,stopTime,a,b,1.0);
    //free elements -end
    //**************************************
}

int main()
{
    printf("\n*************************************************************************\n");
#ifdef STALAB

    printf("test library static");
#endif

#ifdef SHALAB
    printf("test library shared");
#endif

#ifdef DYNLAB
    printf("test library dynamic loaded");
#endif

    clock_t startTime,stopTime;
    struct tms a,b;

    //all - start time
    startTime = clock();
    times(&a);

    //all - do
    struct person tab[INPUT_LENGHT];
    get_data(tab);
    test_list(tab);
    test_tree(tab);

    //all - stop
    stopTime = clock();
    times(&b);
    printf("\nall test ended\n");
    print_time(startTime,stopTime,a,b,1.0);
    //all -end

    printf("\n*************************************************************************\n");
    return 0;
}
