#ifndef LIST_LIB_H
#define LIST_LIB_H
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

/*
 * Inserisce un nuovo nodo in testa alla lista con campo pid = new_pid
 * INPUT:
 * - *p: puntatore a struttura lista_pid
 * - new_pid: valore del pid da inserire nel nuovo nodo
 */
lista_pid *insert_pid(lista_pid *p, pid_t new_pid);

/*
 * Inserisce un nuovo nodo in testa alla lista con campo attraversate = num_attraversate
 * INPUT:
 * - *p: puntatore a struttura lista_pid
 * - num_attraversate: valore del campo attraversate da inserire nel nuovo nodo
 */
lista_pid *insert_attraversate(lista_pid *p, int num_attraversate);

/*
 * Elimina dalla lista la prima occorrenza del nodo con campo attraversate = num_attraversate
 * INPUT:
 * - *p: puntatore a struttura lista_pid
 * - num_attraversate: valore del campo attraversate del nodo da eliminare
 */
lista_pid *delete_attraversate(lista_pid *p, int num_attraversate);

/*
 * Restituisce il nodo minimo della lista in base al valore del campo attraversate
 * INPUT:
 * - *p: puntatore a struttura lista_pid
 */
lista_pid *min_node(lista_pid *p);

/*
 * Restituisce 1 se esiste almeno un nodo con campo pid specificato
 * INPUT:
 * - *p: puntatore a struttura lista_pid
 * - pid: pid da cercare
 */
int check(lista_pid *p, pid_t pid);

/*
 * Stampa la lista dei campi pid di q(testa della lista)
*/
void print_list(struct list *q);

/*
 * Stampa la lista dei campi attraversate di q(testa della lista)
*/
void print_list_attraversate(struct list *q);

#endif /* LIST_LIB_H */