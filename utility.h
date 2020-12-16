#ifndef UTILITY_H
#define UTILITY_H
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#define SO_WIDTH 5
#define SO_HEIGHT 5
#define NUM_RISORSE SO_WIDTH *SO_HEIGHT
#define INDEX(x, y) (y * SO_WIDTH) + x

#define TEST_ERROR                                 \
    if (errno)                                     \
    {                                              \
        fprintf(stderr,                            \
                "%s:%d: PID=%5d: Error %d (%s)\n", \
                __FILE__,                          \
                __LINE__,                          \
                getpid(),                          \
                errno,                             \
                strerror(errno));                  \
    }


typedef struct
{
    int row;
    int column;
} coordinate;

#define REQUEST_LENGTH (sizeof(coordinate) + sizeof(coordinate))

struct msg_request
{
    long mtype;
    /* data */
    coordinate start;
    coordinate end;
};

typedef struct
{
    int crossing_time;
    int nmax_taxi;
    /*pid_t actual_pids[SO_CAP_MAX];*/
    int is_hole;
    int crossing_cont;
} cell;

struct shared_map
{
    cell matrix[SO_HEIGHT][SO_WIDTH];
};

#define SHARED_MAP_LENGTH (sizeof(struct shared_map))

/* --------------SEZIONE STAMPA----------------- */

/*
 * Stampa i valori dell'array di semafori indicato da id_sem
 * INPUT:
 * - id_sem: ID del semaforo
*/
void print_resource(int id_sem);

/*
 * Stampa la matrice del campo indicato
 * INPUT:
 * - mat: puntatore alla strutta shared_map
 * - campo: indice del campo ( 1 = crossing_time ; 2 = crossing_cont ; 3 = is_hole ; 4 = nmax_taxi ) 
*/
void stampa_matrice(struct shared_map * mat, int campo);

#endif /* UTILITY_H */