#include "utility.h"
#include "sem_lib.h"

/* prootipi funzioni */
int creazione();
int spostamento();
void signal_handler();

int main(int argc, char const *argv[])
{
    /* argv[1] = id coda, argv[2] = id semaforo, argv[3] = indice del semaforo nell'array */
    if (argc != 4)
    {
        fprintf(stderr, "Taxi PID:%d: numero di parametri errato.\n", getpid());
        exit(EXIT_FAILURE);
    }

    /* prelievo id della coda e semaforo */
    int id_msg_queue = atoi(argv[1]);
    int id_sem = atoi(argv[2]);
    int index = atoi(argv[3]);

    /* dichiarazione variabili */
    int msg_type;
    int msg_flag;
    struct msg_request request;

    /* creazione signal header */
    struct sigaction sa;
    bzero(&sa, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    sigaction(SIGALRM, &sa, NULL);

    alarm(SO_TIMEOUT);

    /* creazione in posizione casuale */


    /* attesa dello zero */
    wait_sem_zero(id_sem, index);

    /* prelievo richieste con coda */
    msgrcv(id_msg_queue, &request, REQUEST_LENGTH, msg_type, msg_flag);
    TEST_ERROR;

    /* spostamento */
    spostamento();

    /* fine */
    return 0;
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