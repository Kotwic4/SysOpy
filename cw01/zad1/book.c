#include "book.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//yyyy-mm-dd
struct Date* convert_date(char *date_in_string)
{
    struct Date * date = (struct Date*)malloc(sizeof(struct Date));
    char c[3];
    strncpy(c,date_in_string+8,2);
    date->day = atoi(c);
    strncpy(c,date_in_string+5,2);
    date->mon = atoi(c);
    char b[5];
    strncpy(b,date_in_string,4);
    date->year = atoi(b);
    return date;
}

int compare_date(struct Date *a, struct Date *b)
{
    if(a->year > b->year) return -1;
    else if(a->year < b->year) return 1;
    else if(a->mon > b->mon) return -1;
    else if(a->mon < b->mon) return 1;
    else if(a->day > b->day) return -1;
    else if(a->mon < b->mon) return 1;
    else return 0;
}

void free_date(struct Date *a)
{
    free(a);
}

struct Node* create_node(char *first_name,
                         char *last_name,
                         struct Date *birth_date,
                         char *email,
                         char *phone,
                         char *address)
{
    struct Node * node = (struct Node*)malloc(sizeof(struct Node));
    node->first_name = (char*)malloc(sizeof(char)*80);
    node->last_name = (char*)malloc(sizeof(char)*80);
    node->address = (char*)malloc(sizeof(char)*80);
    node->email = (char*)malloc(sizeof(char)*80);
    node->phone = (char*)malloc(sizeof(char)*80);
    strcpy ( node->first_name, first_name);
    strcpy ( node->last_name, last_name);
    strcpy ( node->address, address);
    strcpy ( node->email, email);
    strcpy ( node->phone, phone);
    node->birth_date = birth_date;
    return node;
}

void print_node(struct Node*a)
{
    printf("%s %s %s %s %s %d-%d-%d\n", a->first_name,a->last_name,a->email,a->phone,a->address,a->birth_date->year,a->birth_date->mon,a->birth_date->day);
}

void free_node(struct Node* a)
{
    free_date(a->birth_date);
    free(a->first_name);
    free(a->last_name);
    free(a->email);
    free(a->phone);
    free(a->address);
    free(a);
}

int comp_node(struct Node *a, struct Node *b, enum comp_node_enum type)
{
    switch(type){
        case FIRST_NAME: return strcmp(a->first_name,b->first_name);
        case LAST_NAME: return strcmp(a->last_name,b->last_name);
        case BIRTH_DATE: return compare_date(a->birth_date,b->birth_date);
        case EMAIL: return strcmp(a->email,b->email);
        case PHONE: return strcmp(a->phone,b->phone);
        case ADDRESS: return strcmp(a->address,b->address);
    }
    return 0;
}

int comp_node_value(struct Node *a, char *b, enum comp_node_enum type){
    switch(type){
        case FIRST_NAME: return strcmp(a->first_name,b);
        case LAST_NAME: return strcmp(a->last_name,b);
        case BIRTH_DATE: return compare_date(a->birth_date,convert_date(b));
        case EMAIL: return strcmp(a->email,b);
        case PHONE: return strcmp(a->phone,b);
        case ADDRESS: return strcmp(a->address,b);
    }
    return 0;
}

struct List_node* create_list_node(struct Node *a)
{
    struct List_node * list_node = (struct List_node*)malloc(sizeof(struct List_node));
    list_node->next = NULL;
    list_node->previous = NULL;
    list_node->value = a;
    return list_node;
}

void free_list_node(struct List_node* a)
{
    free_node(a->value);
    free(a);
}

int comp_list_node(struct List_node *a, struct List_node *b, enum comp_node_enum type)
{
    return comp_node(a->value,b->value,type);
}

struct List_book* create_list_book()
{
    struct List_book * list_book = (struct List_book*)malloc(sizeof(struct List_book));
    list_book->head = create_list_node(NULL);
    list_book->tail = create_list_node(NULL);
    list_book->head->next = list_book->tail;
    list_book->tail->previous = list_book->head;
    return list_book;
}

void free_list_book(struct List_book *a)
{
    struct List_node* p = a->head->next;
    while(p != a->tail){
        p = p->next;
        free_list_node(p->previous);
    }
    free(a->head);
    free(a->tail);
    free(a);
}

void add_node_list_book(struct List_book *a, struct Node *b)
{
    struct List_node* node = create_list_node(b);
    node->next = a->head->next;
    a->head->next = node;
    node->next->previous = node;
    node->previous = a->head;
}

void remove_node_list_book(struct List_book *a, struct Node *b)
{
    struct List_node* p = a->head->next;
    while(p != a->tail){
        if(p->value == b){
            p->next->previous = p->previous;
            p->previous->next = p->next;
            free_list_node(p);
            return;
        }
        p = p->next;
    }
}

struct Node* find_node_list_book(struct List_book *a, char *value, enum comp_node_enum type)
{
    struct List_node* p = a->head->next;
    while(p != a->tail){
        if(comp_node_value(p->value,value,type) == 0){
            return p->value;
        }
        p = p->next;
    }
    return NULL;
}

void merge_list_book(struct List_book *a, struct List_book *b)
{
    a->tail->previous->next = b->head->next;
    b->head->next->previous = a->tail->previous;
    b->head->next = b->tail;

    b->tail->previous->next = a->tail;
    a->tail->previous = b->tail->previous;
    b->tail->previous = b->head;

    free_list_book(b);
}

void sort_list_book(struct List_book *a, enum comp_node_enum type)
{
    struct List_node* x = a->head->next;
    if(x == a->tail) return;
    struct List_book* b = create_list_book();
    struct List_book* c = create_list_book();
    a->head->next = x->next;
    x->next->previous = a->head;
    x->next = NULL;
    x->previous = NULL;
    struct List_node* p = a->head->next;
    struct List_node* q;
    while(p != a->tail){
        q = p;
        p = p->next;
        int comp = comp_list_node(q,x,type);
        if(comp >= 0){
            q->previous->next = p;
            p->previous = q->previous;
            if(comp == 0) add_node_list_book(b,q->value);
            else add_node_list_book(c,q->value);
            free(q);
        }
    }
    add_node_list_book(b,x->value);
    free(x);
    sort_list_book(a,type);
    sort_list_book(c,type);
    merge_list_book(a,b);
    merge_list_book(a,c);
}

struct Tree_node* create_tree_node(struct Node *a)
{
    struct Tree_node * node = (struct Tree_node*)malloc(sizeof(struct Tree_node));
    node->value = a;
    node->left = NULL;
    node->right = NULL;
    node->p = NULL;
    return node;
}

void free_tree_node(struct Tree_node* a)
{
    if(a == NULL) return;
    free_node(a->value);
    free_tree_node(a->left);
    free_tree_node(a->right);
    free(a);
}

int comp_tree_node(struct Tree_node *a, struct Tree_node *b, enum comp_node_enum type)
{
    return comp_node(a->value,b->value,type);
}

struct Tree_book* create_tree_book(enum comp_node_enum type)
{
    struct Tree_book * tree_book = (struct Tree_book*)malloc(sizeof(struct Tree_book));
    tree_book->type = type;
    tree_book->root = NULL;
    return tree_book;
}

void free_tree_book(struct Tree_book *a)
{
    free_tree_node(a->root);
    free(a);
}

void add_node_tree_book(struct Tree_book *a, struct Node *b)
{
    struct Tree_node* x = create_tree_node(b);
    if(a->root == NULL){
        a->root = x;
    }
    else{
        struct Tree_node* p = a->root;
        while(1){
            int comp = comp_node(b,p->value,a->type);
            if(comp < 0){
                if(p->left == NULL){
                    p->left = x;
                    x->p=p;
                    return;
                }
                else{
                    p = p->left;
                    continue;
                }
            }
            else{
                if(p->right == NULL){
                    p->right = x;
                    x->p=p;
                    return;
                }
                else{
                    p = p->right;
                    continue;
                }
            }
        }
    }

}

struct Tree_node * find_pred(struct Tree_node *x)
{
    if(x->left){
        x = x->left;
        while(x->right) x = x->right;
        return x;
    }
    struct Tree_node *y;
    do{
        y = x;
        x = x->p;
    }while(x && (x->right != y));
    return x;
}

struct Tree_node *  remove_node_tree(struct Tree_book *a,struct Tree_node *x)
{
    struct Tree_node* y = x->p,*z=NULL;
    if((x->left!=NULL)&&(x->right!=NULL))
    {
        z = remove_node_tree(a,find_pred(x));
        z->left = x->left;
        if(z->left)  z->left->p  = z;
        z->right = x->right;
        if(z->right) z->right->p = z;
    }
    else{
        if(x->left){
            z = x->left;
        }
        else{
            z = x->right;
        }
    }
    if(z!=NULL) z->p = y;
    if(y==NULL) a->root = z;
    else{
        if(y->left == x) y->left = z;
        else y->right = z;
    }
    return x;
}

void remove_node_tree_book(struct Tree_book *a, struct Node *b)
{
    struct Tree_node* x = a->root;
    while(1){
        int comp = comp_node(b,x->value,a->type);
        if(comp == 0) break;
        if(comp < 0){
            x = x->left;
        }
        else{
            x = x->right;
        }
        if(x == NULL) return;
    }
    x = remove_node_tree(a,x);
    x->left = NULL;
    x->right = NULL;
    x->p = NULL;
    free_tree_node(x);
}

struct Node* find_node_tree_book(struct Tree_book *a, char *value)
{
    struct Tree_node* p = a->root;
    while(p != NULL){
        int comp = comp_node_value(p->value,value,a->type);
        if(comp == 0) return p->value;
        comp = comp * (-1);
        if(comp < 0){
            p = p->left;
        }
        else{
            p = p->right;
        }
    }
    return NULL;
}

void move_tree_node(struct Tree_book *a,struct Tree_node *x)
{
    if(x==NULL) return;
    else{
        move_tree_node(a,x->left);
        move_tree_node(a,x->right);
        add_node_tree_book(a,x->value);
        free(x);
    }
}

void rebuild_tree_book(struct Tree_book *a, enum comp_node_enum type)
{
    struct Tree_node *x = a->root;
    a->root = NULL;
    a->type = type;
    move_tree_node(a,x);
}

void print_list(struct List_book *a)
{
    printf("wypisanie listy\n");
    struct List_node*x = a->head->next;
    while(x != a->tail){
        print_node(x->value);
        x = x->next;
    }
    printf("\n");
}

void print_tree_node(struct Tree_node *a,int depth)
{
    int x = depth+1;
    if(a == NULL) return;
    print_tree_node(a->left,x);
    printf("%d ", depth);
    print_node(a->value);
    print_tree_node(a->right,x);
}

void print_tree(struct Tree_book *a)
{
    printf("wypisanie drzewa\n");
    print_tree_node(a->root,0);
    printf("\n");
}


