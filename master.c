#include "sem_lib.h"
#include "utility.h"

#ifdef DENSE
unsigned int SO_HOLES = 10;
unsigned int SO_SOURCES;
unsigned int SO_CAP_MIN;
unsigned int SO_CAP_MAX;
unsigned int SO_TAXI;
unsigned int SO_TIMENSEC_MIN;
unsigned int SO_TIMENSEC_MAX;
unsigned int SO_TIMEOUT;
unsigned int SO_DURATION;
#endif
#ifdef LARGE
unsigned int SO_HOLES = 50;
unsigned int SO_SOURCES;
unsigned int SO_CAP_MIN;
unsigned int SO_CAP_MAX;
unsigned int SO_TAXI;
unsigned int SO_TIMENSEC_MIN;
unsigned int SO_TIMENSEC_MAX;
unsigned int SO_TIMEOUT;
unsigned int SO_DURATION;
#endif
int main(int argc, char const *argv[])
{
    /* parametri definiti a tempo di esecuzione */
    unsigned int SO_HOLES;        /* 10 - 50 */
    unsigned int SO_SOURCES;      /* SO_WIDTH×SO_HEIGHT−SO_HOLES - 10 */
    unsigned int SO_CAP_MIN;      /* 1 - 3 */
    unsigned int SO_CAP_MAX;      /* 1 - 5 */
    unsigned int SO_TAXI;         /* SO_SOURCES/2 - 1000 */
    unsigned int SO_TIMENSEC_MIN; /* [nsec] 100000000 - 10000000 */
    unsigned int SO_TIMENSEC_MAX; /* [nsec] 300000000 - 100000000 */
    unsigned int SO_TIMEOUT;      /* [sec] 1 - 3 */
    unsigned int SO_DURATION;     /* [sec] 20 - 20 */

    return 0;
}
