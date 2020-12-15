#include "sem_lib.h"
#include "utility.h"

/* parametri definiti a tempo di compilazione */
#ifdef DENSE
#define SO_WIDTH 20
#define SO_HEIGHT 10
#elif LARGE
#define SO_WIDTH 60
#define SO_HEIGHT 20
#else
#define SO_WIDTH 10
#define SO_HEIGHT 10
#endif

/* prototipi di funzioni */
void creaMatrice();

/* variabili globali */
unsigned int SO_HOLES;
unsigned int SO_TOP_CELLS;
unsigned int SO_SOURCES;
unsigned int SO_CAP_MIN;
unsigned int SO_CAP_MAX;
unsigned int SO_TAXI;
unsigned int SO_TOP_CELLS;
unsigned long int SO_TIMENSEC_MIN;
unsigned long int SO_TIMENSEC_MAX;
unsigned int SO_TIMEOUT;
unsigned int SO_DURATION;

struct shared_map *city;

int main(int argc, char const *argv[])
{
    printf("\nSO_HOLES: %d", SO_HOLES);
    printf("\nSO_TOP_CELLS: %d", SO_TOP_CELLS);
    printf("\nSO_SOURCES: %d", SO_SOURCES);
    printf("\nSO_CAP_MIN: %d", SO_CAP_MIN);
    printf("\nSO_CAP_MAX: %d", SO_CAP_MAX);
    printf("\nSO_TAXI: %d", SO_TAXI);
    printf("\nSO_TIMENSEC_MIN: %lu", SO_TIMENSEC_MIN);
    printf("\nSO_TIMENSEC_MAX: %lu", SO_TIMENSEC_MAX);
    printf("\nSO_TIMEOUT: %d", SO_TIMEOUT);
    printf("\nSO_DURATION: %d\n", SO_DURATION);

    /* dichiarazione variabili */
    int id_shd_mem;

    /* creazione memoria condivisa */
    id_shd_mem = shmget(IPC_PRIVATE, SHARED_MAP_LENGTH, IPC_CREAT | IPC_EXCL | 0664);
    TEST_ERROR;

    /* attacching memoria condivisa */
    city = shmat(id_shd_mem, NULL, 0);
    TEST_ERROR;

    /* detaching ed eliminazione memoria condivisa */
    shmdt(city);
    shmctl(id_shd_mem, IPC_RMID, 0);
    TEST_ERROR;

    return 0;
}
