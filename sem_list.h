#ifndef SEM_LIST_H
#define SEM_LIST_H
#define _GNU_SOURCE

#include "utility.h"


struct list
{
    pid_t pid;
    coordinate posizione;
    struct list *next;
    int attraversate;
};

typedef struct list lista_pid;

lista_pid *insert_pid(lista_pid *p, pid_t new_pid);

lista_pid *insert_attraversate(lista_pid *p, int num_attraversate);

lista_pid *delete_attraversate(lista_pid *p, int num_attraversate);

lista_pid *min_node(lista_pid *p);

int check(lista_pid *p, pid_t pid);

void print_list(struct list *q);

void print_list_attraversate(struct list *q);

#endif