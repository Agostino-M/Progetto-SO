#ifndef SEM_LIST_H
#define SEM_LIST_H
#define _GNU_SOURCE

#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>

struct list
{
    pid_t pid;
    struct list *next;
};

typedef struct list lista_pid;

lista_pid *create_list(void);
lista_pid *insert(lista_pid *p, pid_t new_pid);
lista_pid *delete_pid(lista_pid *p, pid_t pid);
int check(lista_pid *p, pid_t pid);
void print_list(struct list *q);

#endif