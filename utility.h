#ifndef UTILITY_H
#define UTILITY_H
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <signal.h>

#define SO_WIDTH 20
#define SO_HEIGHT 10

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

#define REQUEST_LENGTH (sizeof(coordinate) + sizeof(coordinate))
typedef struct
{
    int row;
    int column;
} coordinate;

struct msg_request
{
    long mtype;
    /* data */
    coordinate start;
    coordinate end;
};

struct Node
{
    int data;
    struct Node *next;
};

typedef struct
{
    int crossing_time;
    int nmax_taxi;
    struct Node actual_pids;
    int is_hole;
} cell;

struct shared_map
{
    cell matrix[SO_WIDTH][SO_HEIGHT];
    cell taxi[SO_WIDTH][SO_HEIGHT];
};

#endif /* UTILITY_H */