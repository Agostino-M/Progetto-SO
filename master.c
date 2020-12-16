#include "sem_lib.h"
#include "utility.h"
#include "handling.h"

/* parametri definiti a tempo di compilazione
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
*/

/* prototipi di funzioni */
void creaMatrice();

/* variabili globali */
unsigned int SO_HOLES = 10;
unsigned int SO_SOURCES = 190;
unsigned int SO_CAP_MIN = 1;
unsigned int SO_CAP_MAX = 1;
unsigned int SO_TAXI = 95;
/* unsigned int SO_TOP_CELLS; */
unsigned long int SO_TIMENSEC_MIN = 100000000;
unsigned long int SO_TIMENSEC_MAX = 300000000;
unsigned int SO_TIMEOUT = 1;
unsigned int SO_DURATION = 20;

typedef struct
{
    int crossing_time;
    int nmax_taxi;
    /*pid_t actual_pids[SO_CAP_MAX];*/
    int is_hole;
} cell;

struct shared_map
{
    cell matrix[SO_HEIGHT][SO_WIDTH];
};

#define SHARED_MAP_LENGTH (sizeof(struct shared_map))

struct shared_map *city;

int main(int argc, char const *argv[])
{
    /*printf("\nSO_HOLES: %d", SO_HOLES);
    printf("\nSO_TOP_CELLS: %d", SO_TOP_CELLS);
    printf("\nSO_SOURCES: %d", SO_SOURCES);
    printf("\nSO_CAP_MIN: %d", SO_CAP_MIN);
    printf("\nSO_CAP_MAX: %d", SO_CAP_MAX);
    printf("\nSO_TAXI: %d", SO_TAXI);
    printf("\nSO_TIMENSEC_MIN: %lu", SO_TIMENSEC_MIN);
    printf("\nSO_TIMENSEC_MAX: %lu", SO_TIMENSEC_MAX);
    printf("\nSO_TIMEOUT: %d", SO_TIMEOUT);
    printf("\nSO_DURATION: %d\n", SO_DURATION);
    */

    /* dichiarazione variabili */
    int id_shd_mem;

    /* creazione signal header */
    struct sigaction sa;
    bzero(&sa, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    sigaction(SIGALRM, &sa, NULL);

    /* creazione memoria condivisa */
    id_shd_mem = shmget(IPC_PRIVATE, SHARED_MAP_LENGTH, IPC_CREAT | IPC_EXCL | 0666);
    TEST_ERROR;

    /* attacching memoria condivisa */
    city = shmat(id_shd_mem, NULL, 0);
    TEST_ERROR;

    /* creazione matrice */
    creaMatrice();

    /* creazione richieste */

    /* creazione taxi */

    /* semaforo wait for zero */

    /* parte il timer SO_DURATION */
    alarm(SO_DURATION);

    /* detaching ed eliminazione memoria condivisa */
    shmdt(city);
    shmctl(id_shd_mem, IPC_RMID, 0);
    TEST_ERROR;

    return 0;
}

void creaMatrice()
{
    int i, j, i_holes, j_holes;
    unsigned long int random;

    srand(getpid());

    for (i = 0; i < SO_HEIGHT; i++)
    {
        for (j = 0; j < SO_WIDTH; j++)
        {
            random = rand() % SO_TIMENSEC_MAX + SO_TIMENSEC_MIN;
            city->matrix[i][j].crossing_time = random;
            city->matrix[i][j].is_hole = 0;
        }
    }

    /* stampa */
    for (i = 0; i < SO_HEIGHT; i++)
    {
        for (j = 0; j < SO_WIDTH; j++)
        {
            printf("%d ", city->matrix[i][j].is_hole);
            if (j == SO_WIDTH - 1)
            {
                printf("\n");
            }
        }
    }
}
