#include "sem_list.h"

int check(lista_pid *p, pid_t check_pid)
{
    int cont = 0;
    lista_pid *q;

    while (p != NULL)
    {
        if (p->pid == check_pid)
        {
            return cont;
        }
        p = p->next;
        cont++;
    }

    return -1;
}

lista_pid *create_list(void)
{
    return NULL;
}

lista_pid *insert(lista_pid *p, pid_t new_pid)
{
    lista_pid *q = malloc(sizeof(lista_pid));

    if (!q)
    {
        fprintf(stderr, "Errore allocazione del nuovo elemento\n");
        exit(-1);
    }
    q->pid = new_pid;
    q->next = p;
    return q;
}

lista_pid *delete_pid(lista_pid *head, pid_t value)
{
    lista_pid *p = head, *q;

    /*Lista vuota*/
    if (head == NULL)
        return NULL;

    /*Elemento da cancellare in testa*/

    if (head->pid == value)
    {
        head = head->next;
        free(p);
        return head;
    }

    /*Ciclo alla ricerca dell'elemento con pid == value */
    for (p = head; p->next != NULL; p = p->next)
    {
        if (p->next->pid == value)
        {
            /*Trovato elemento da cancellare*/
            q = p->next->next;
            free(p->next);
            p->next = q;
            break;
        }
    }

    return head;
}

void print_list(struct list *q)
{
    lista_pid *temp = q;

    while (temp != NULL)
    {
        printf("%d ", temp->pid);
        temp = temp->next;
    }
}

