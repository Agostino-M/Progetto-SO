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

#define REQUEST_LENGTH (sizeof(void) + sizeof(void))
struct msg_request
{
    long mtype;
    /* data */
    coordinate start;
    coordinate end;
};

struct cell
{
    int crossing_time;
    int nmax_taxi;
};

#endif /* UTILITY_H */