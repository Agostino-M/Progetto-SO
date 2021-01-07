#ifndef UTILITY_H
#define UTILITY_H
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define FILEPATH "./Taxi"
#define SO_WIDTH 20
#define SO_HEIGHT 10
#define NUM_RISORSE SO_WIDTH *SO_HEIGHT
#define QUARTIERE 3
#define INDEX(x, y) (((x) * (SO_WIDTH)) + (y))

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define TEST_ERROR                                \
    if (errno)                                    \
    {                                             \
        fprintf(stderr,                           \
                "%s:%d: PID:%d: Error %d (%s)\n", \
                __FILE__,                         \
                __LINE__,                         \
                getpid(),                         \
                errno,                            \
                strerror(errno));                 \
        exit(EXIT_FAILURE);                       \
    }

typedef struct
{
    int x;
    int y;
} coordinate;

#define REQUEST_LENGTH (sizeof(coordinate) + sizeof(coordinate))

struct msg_request
{
    long mtype;
    coordinate start;
    coordinate end;
};

typedef struct
{
    long crossing_time;
    int nmax_taxi;
    int is_hole;
    int crossing_cont;
    int is_top;
    pid_t request_pid;
} cell;

struct shared_map
{
    cell matrix[SO_HEIGHT][SO_WIDTH];
};

struct shared_stats
{
    int num_viaggi_eseguiti;
    int num_viaggi_abortiti;
    int max_strada_fatta;
    pid_t pid_max_strada_fatta;
    long max_viaggio;
    pid_t pid_max_viaggio;
    int max_richieste;
    pid_t pid_max_richieste;
};

struct top_cells
{
    int attraversamenti;
    coordinate posizione;
};

#define SHARED_MAP_LENGTH (sizeof(struct shared_map))
#define SHARED_STATS_LENGHT (sizeof(struct shared_stats))

/* -----------------SEZIONE STAMPA----------------- */

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
 * - campo: indice del campo (1: crossing_time; 2: crossing_cont; 3: is_hole; 4: nmax_taxi; 5: request_pid) 
*/
void print_matrix(struct shared_map *mat, int field);

void print_status(struct shared_map *mat, int id_sem_cap);

int min_vet(struct top_cells *vet, int dim);

#endif /* UTILITY_H */