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

/* Potrebbe non servire
 * #define INDEX(x,y) (y * SO_WIDTH) + x
 */

#endif /* UTILITY_H */