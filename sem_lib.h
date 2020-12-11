#ifndef SEM_LIB_H
#define SEM_LIB_H
#define _GNU_SOURCE

#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>

union semun
{
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO
				    (Linux-specific) */
};

/*
 * Setta il semaforo al valore definito dall'utente
 * INPUT:
 * - sem_id: ID del semaforo
 * - index: posizione del semaforo nell'array
 * - sem_val: valore di inizializzazione del semaforo
 * RESULT:
 * - semaforo inizializzato al valore sem_val
 * - il valore di ritorno è lo stesso dell'invocazione di semctl
 */
int set_sem(int sem_id, int index, int sem_val);

/*
 * Tenta di accedere alla risorsa
 * INPUT:
 * - sem_id: ID del semaforo
 * - index: posizione del semaforo nell'array
 * RISULTATO
 * - Se la risorsa è disponibile il semaforo è decrementato di 1
 * - Se la risorsa non è disponibile (valore semaforo = 0), il
 *   processo è bloccato finchè la risorsa non è nuovamente disponibile
 */
void dec_sem(int sem_id, int index);

/*
 * Rilascia la risorsa
 * INPUT:
 * - sem_id: ID del semaforo
 * - index: posizione del semaforo nell'array
 * RISULTATO:
 * - Semaforo incrementato di 1. Potrebbe sbloccare qualche processo
 */
void rel_sem(int sem_id, int index);

/*
 * Il valore del semaforo è testato per vedere se attualmente è uguale a 0
 * INPUT:
 * - sem_id : ID del semaforo
 * - index : posizione del semaforo nell'array
 * RISULTATO:
 * - Se il valore del semaforo è 0 l'operazione è completata immediatamente
 * - Se il valore del semaforo è diverso da 0 si blocca finchè il valore del semaforo diviene 0
*/
void wait_sem_zero(int sem_id, int index);

/*
 * Setta il valore di tutti i semafori dell'array a 1
 * INPUT:
 * - sem_id : ID del semaforo
 * - dim : dimensione dell'array di semafori
*/
void reset_sem_set(int sem_id, int dim);

#endif /* SEM_LIB_H */