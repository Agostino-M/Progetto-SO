#ifndef UTILITY_H
#define UTILITY_H

#define SO_WIDTH 20  /* dense 20- large 60 */
#define SO_HEIGHT 10 /* dense 10- large 20 */

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

#endif /* UTILITY_H */