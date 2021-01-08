#include "sem_list.h"

lista_pid *insert_pid(lista_pid *p, pid_t new_pid)
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

lista_pid *insert_attraversate(lista_pid *p, int num_attraversate)
{
    lista_pid *q = malloc(sizeof(lista_pid));

    if (!q)
    {
        fprintf(stderr, "Errore allocazione del nuovo elemento\n");
        exit(-1);
    }
    q->attraversate = num_attraversate;
    q->next = p;
    return q;
}

lista_pid *delete_attraversate(lista_pid *head, int num_attraversate)
{
    lista_pid *p = head, *q;

    /*Lista vuota*/
    if (head == NULL)
        return NULL;

    /*Elemento da cancellare in testa*/

    if (head->attraversate == num_attraversate)
    {
        head = head->next;
        free(p);
        return head;
    }

    /*Ciclo alla ricerca dell'elemento con pid == value */
    for (p = head; p->next != NULL; p = p->next)
    {
        if (p->next->attraversate == num_attraversate)
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

void print_list_attraversate(struct list *q)
{
    lista_pid *temp = q;

    while (temp != NULL)
    {
        printf("%d ", temp->attraversate);
        temp = temp->next;
    }
}

lista_pid *min_node(lista_pid *p)
{
    lista_pid *q = p;
    int min, cont = 0, position = 0, i;

    min = q->attraversate;

    while (q != NULL)
    {
        if (min > q->attraversate)
        {
            min = q->attraversate;
            position = cont;
        }
        cont++;
        q = q->next;
    }

    q = p;

    for (i = 0; i < position; i++)
    {
        q = q->next;
    }

    return q;
}
