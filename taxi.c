#include "utility.h"
#include "sem_lib.h"

/* prootipi funzioni */
void creazione();
int spostamento();
void signal_handler();

int id_shd_mem;
int id_msg_queue;
int id_sem_cap;
int id_sem_taxi;

int main(int argc, char const *argv[])
{
    /* controllo dei parametri ricevuti */
    if (argc != 5)
    {
        fprintf(stderr, "Taxi PID:%d: numero di parametri errato.\n", getpid());
        exit(EXIT_FAILURE);
    }

    /* 
     * prelievo argomenti argv 
     * - argv[1] = id memoria condivisa
     * - argv[2] = id coda di messaggi per le richieste
     * - argv[3] = id semaforo delle capacità massime per taxi su una cella
     * - argv[4] = id semaforo "wait for zero" che dà il via ai taxi
     */
    id_shd_mem = atoi(argv[1]);
    id_msg_queue = atoi(argv[2]);
    id_sem_cap = atoi(argv[3]);
    id_sem_taxi = atoi(argv[4]);

    /* dichiarazione variabili */
    int msg_type;
    int msg_flag;
    struct msg_request request;
    struct shared_map *city;

    /* creazione signal header */
    struct sigaction sa;
    bzero(&sa, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    sigaction(SIGALRM, &sa, NULL);

    /* attacching memoria condivisa */
    city = shmat(id_shd_mem, NULL, 0);
    TEST_ERROR

    /* creazione in posizione casuale */
    creazione();

    /* semaforo wait for zero */
    wait_sem_zero(id_sem_taxi, index);

    /* prelievo richieste con coda */
    msgrcv(id_msg_queue, &request, REQUEST_LENGTH, msg_type, msg_flag);
    TEST_ERROR

    /* parte il timer SO_TIMEOUT */
    alarm(SO_TIMEOUT);

    /* spostamento */
    spostamento();

    /* detaching memoria condivisa */
    shmdt(city);
    TEST_ERROR

    /* fine */
    return 0;
}

void creazione()
{
    int random_x;
    int random_y;

    srand(getpid());

    while ()
    {
        random_x = rand() % SO_HEIGHT;
        random_y = rand() % SO_WIDTH;

        /* controllare semaforo della cella */
        do
        {
            /* code */

        } while (errno == EAGAIN);

        /* aggiornamento campi della matrice condivisa */
    }
}

int spostamento()
{
}

void signal_handler(int signum)
{
    if (signum == SIGALRM)
    {
        printf("Taxi PID:%d: Timer scaduto...\n", getpid());
        /* - gestire caso in cui stava gestendo una richiesta
         * - gestire rilascio di risorse (semafori)
        */
    }
}