#ifndef _BOOK_H
#define _BOOK_H




//comp = -1 a < b
//comp = 0 a = b
//comp = 1 a > b

struct Date{
    int day;
    int mon;
    int year;
};

struct Date* convert_date(char *date_in_string);
int compare_date(struct Date *a, struct Date *b);
void free_date(struct Date *a);

struct Node{
    char *first_name;
    char *last_name;
    struct Date *birth_date;
    char *email;
    char *phone;
    char *address;
};

enum comp_node_enum{
    FIRST_NAME,
    LAST_NAME,
    BIRTH_DATE,
    EMAIL,
    PHONE,
    ADDRESS
};

struct Node* create_node(char *first_name,
                       char *last_name,
                       struct Date *birth_date,
                       char *email,
                       char *phone,
                       char *address);
void print_node(struct Node*a);
void free_node(struct Node* a);
int comp_node(struct Node *a, struct Node *b, enum comp_node_enum type);

struct List_node{
    struct Node *value;
    struct List_node *next;
    struct List_node *previous;
};

struct List_node* create_list_node(struct Node *a);
void free_list_node(struct List_node* a);
int comp_list_node(struct List_node *a, struct List_node *b, enum comp_node_enum type);
int comp_node_value(struct Node *a, char *b, enum comp_node_enum type);

struct List_book{
    struct List_node *head;
    struct List_node *tail;
};

struct List_book* create_list_book();
void free_list_book(struct List_book *a);
void print_list(struct List_book *a);
void add_node_list_book(struct List_book *a, struct Node *b);
void remove_node_list_book(struct List_book *a, struct Node *b);
struct Node* find_node_list_book(struct List_book *a, char *value, enum comp_node_enum type);
void merge_list_book(struct List_book *a, struct List_book *b);
void sort_list_book(struct List_book *a, enum comp_node_enum type);

struct Tree_node{
    struct Node *value;
    struct Tree_node *p;
    struct Tree_node *left;
    struct Tree_node *right;
};

struct Tree_node* create_tree_node(struct Node *a);
void free_tree_node(struct Tree_node* a);
int comp_tree_node(struct Tree_node *a, struct Tree_node *b, enum comp_node_enum type);

struct Tree_book{
    enum comp_node_enum type;
    struct Tree_node *root;
};

struct Tree_book* create_tree_book(enum comp_node_enum type);
void print_tree(struct Tree_book *a);
void free_tree_book(struct Tree_book *a);
void add_node_tree_book(struct Tree_book *a, struct Node *b);
void remove_node_tree_book(struct Tree_book *a, struct Node *b);
struct Node* find_node_tree_book(struct Tree_book *a, char *value);
void rebuild_tree_book(struct Tree_book *a, enum comp_node_enum type);

#endif
