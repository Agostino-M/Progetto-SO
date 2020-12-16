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
void fill_resource();

/* variabili globali */
unsigned int SO_HOLES = 2;
unsigned int SO_SOURCES = 190;
unsigned int SO_CAP_MIN = 1;
unsigned int SO_CAP_MAX = 10;
unsigned int SO_TAXI = 95;
/* unsigned int SO_TOP_CELLS; */
unsigned long int SO_TIMENSEC_MIN = 100000000;
unsigned long int SO_TIMENSEC_MAX = 300000000;
unsigned int SO_TIMEOUT = 1;
unsigned int SO_DURATION = 20;

/* ID dell'IPC del semaforo e` globale */
int id_sem_cap, id_sem_taxi;

struct shared_map *city;

int main(int argc, char const *argv[])
{
    /* dichiarazione variabili */
    int id_shd_mem;

    /* creazione signal header
    struct sigaction sa;
    bzero(&sa, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    sigaction(SIGALRM, &sa, NULL);
    */

    /* creazione memoria condivisa */
    id_shd_mem = shmget(IPC_PRIVATE, SHARED_MAP_LENGTH, IPC_CREAT | IPC_EXCL | 0666);
    TEST_ERROR;

    /* attacching memoria condivisa */
    city = shmat(id_shd_mem, NULL, 0);
    TEST_ERROR;

    /* creazione array semafori capienza */
    id_sem_cap = semget(IPC_PRIVATE, NUM_RISORSE, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR

    /* creazione matrice */
    creaMatrice();
    fill_resource();
    print_resource(id_sem_cap);

    /* creazione richieste */

    /* creazione taxi */

    /* semaforo wait for zero */

    /* parte il timer SO_DURATION */
    alarm(SO_DURATION);

    /* detaching ed eliminazione memoria condivisa */
    shmdt(city);
    shmctl(id_shd_mem, IPC_RMID, 0);
    TEST_ERROR

    /* Eliminazione semafori */
    semctl(id_sem_cap, IPC_RMID, 0);
    semctl(id_sem_taxi, IPC_RMID, 0);

    return 0;
}

void creaMatrice()
{
    int i, j, z, i_holes, j_holes, hole_inseriti = 0, cond = 1;
    unsigned long int random;

    srand(getpid());

    for (i = 0; i < SO_HEIGHT; i++)
    {
        for (j = 0; j < SO_WIDTH; j++)
        {
            random = rand() % SO_TIMENSEC_MAX + SO_TIMENSEC_MIN;
            city->matrix[i][j].crossing_time = random;
            city->matrix[i][j].is_hole = 0;
            city->matrix[i][j].crossing_cont = 0;
        }
    }

    srand(time(NULL));

    for (z = 0; z < SO_HOLES; z++)
    {
        cond = 1;
        i_holes = rand() % SO_HEIGHT;
        j_holes = rand() % SO_WIDTH;

        if (i_holes == 0) /* caso particolare */
            i = i_holes;
        else
            i = i_holes - 1;

        for (i; i <= i_holes + 1 && cond == 1; i++)
        {
            if (j_holes == 0) /* caso particolare */
                j = j_holes;
            else
                j = j_holes - 1;

            for (j; j <= j_holes + 1; j++)
            {
                if (city->matrix[i][j].is_hole == 1)
                {
                    cond = 0;
                }
            }
        }

        i = 0;
        j = 0;

        if (cond == 1)
        {
            city->matrix[i_holes][j_holes].is_hole = 1;
            hole_inseriti++;
        }

        else
        {
            z--;
            continue;
        }
    }

    stampa_matrice(city, 3);
}

void fill_resource()
{
    struct sembuf sops[NUM_RISORSE];
    int i, j, random;
    srand(time(NULL));

    for (i = 0; i < NUM_RISORSE; i++)
    {
        random = (rand() % SO_CAP_MAX) + SO_CAP_MIN;
        sops[i].sem_num = i;
        sops[i].sem_op = random;
    }
    semop(id_sem_cap, sops, NUM_RISORSE);
    TEST_ERROR
}
