#include "utility.h"
#include "sem_lib.h"

/* Prototipi funzioni */
int create_taxi();
void signal_handler();
int move();
int move_up(int x, int y);
int move_down(int x, int y);
int move_left(int x, int y);
int move_right(int x, int y);

int id_shd_mem;
int id_msg_queue;
int id_sem_cap;
int id_sem_taxi;
int id_sem_request;

struct shared_map *city;

coordinate actual_position; /* Eventualmente con struct */

int main(int argc, char const *argv[])
{
    /* Dichiarazione variabili */
    struct msg_request request;
    struct sigaction sa;

    /* Controllo dei parametri ricevuti */
    if (argc != 5)
    {
        fprintf(stderr, "Taxi PID:%d: numero di parametri errato.\n", getpid());
        exit(EXIT_FAILURE);
    }

    /* 
     * Prelievo argomenti argv 
     * - argv[1] = id memoria condivisa
     * - argv[2] = id coda di messaggi per le richieste
     * - argv[3] = id semaforo delle capacità massime per taxi su una cella
     * - argv[4] = id semaforo "wait for zero" che dà il via ai taxi
     * - argv[5] = id semaforo che indica la presenza di una richiesta
     */
    id_shd_mem = atoi(argv[1]);
    id_msg_queue = atoi(argv[2]);
    id_sem_cap = atoi(argv[3]);
    id_sem_taxi = atoi(argv[4]);
    id_sem_request = atoi(argv[5]);

    /* Creazione signal header */
    bzero(&sa, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    sigaction(SIGALRM, &sa, NULL);

    /* Attacching memoria condivisa */
    city = shmat(id_shd_mem, NULL, 0);
    TEST_ERROR

    /* Creazione in posizione casuale */
    if (create_taxi() == -1)
        fprintf(stderr, "Taxi PID:%d: Impossibile effettuare la creazione.\n", getpid());

    /* Semaforo wait for zero */
    wait_sem_zero(id_sem_taxi, 0);

    /* Prelievo richieste con coda */
    dec_sem(id_sem_request, INDEX(actual_position.x, actual_position.y));

    msgrcv(id_msg_queue, &request, REQUEST_LENGTH, city->matrix[actual_position.x][actual_position.y].request_pid, 0);
    TEST_ERROR

    /* Parte il timer SO_TIMEOUT */
    alarm(10); /* SO_TIMEOUT */

    /* Spostamento */
    move();

    /* Detaching memoria condivisa */
    shmdt(city);
    TEST_ERROR

    /* Fine */
    return 0;
}

int create_taxi()
{
    int random_x, random_y, attempts = 0;

    srand(getpid());

    do
    {
        random_x = rand() % SO_HEIGHT;
        random_y = rand() % SO_WIDTH;

        if (!city->matrix[random_x][random_y].is_hole)
            dec_sem_nw(id_sem_cap, INDEX(random_x, random_y));

    } while (errno == EAGAIN || city->matrix[random_x][random_y].is_hole || attempts < 30);

    if (attempts == 30)
        return -1;

    /* Aggiorno le posizioni attuali del taxi */
    actual_position.x = random_x;
    actual_position.y = random_y;

    /* Aggiornamento campi della matrice condivisa */
    city->matrix[random_x][random_y].crossing_cont++;
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

int move()
{
    /* usare move_up/down/left/right per arrivare alla destinazione :) 
     * https://rosettacode.org/wiki/A*_search_algorithm#C
     */
}

int move_up(int x, int y)
{
    if (city->matrix[x][y + 1].is_hole)
        return -1;

    dec_sem(id_sem_cap, INDEX(x, y + 1));

    city->matrix[x][y + 1].crossing_cont++;
    sleep(city->matrix[x][y + 1].crossing_time);

    return 0;
}

int move_down(int x, int y)
{
    if (city->matrix[x][y - 1].is_hole)
        return -1;

    dec_sem(id_sem_cap, INDEX(x, y - 1));

    city->matrix[x][y - 1].crossing_cont++;
    sleep(city->matrix[x][y - 1].crossing_time);

    return 0;
}

int move_left(int x, int y)
{
    if (city->matrix[x - 1][y].is_hole)
        return -1;

    dec_sem(id_sem_cap, INDEX(x - 1, y));

    city->matrix[x - 1][y].crossing_cont++;
    sleep(city->matrix[x - 1][y].crossing_time);

    return 0;
}

int move_right(int x, int y)
{
    if (city->matrix[x + 1][y].is_hole)
        return -1;

    dec_sem(id_sem_cap, INDEX(x + 1, y));

    city->matrix[x + 1][y].crossing_cont++;
    sleep(city->matrix[x + 1][y].crossing_time);

    return 0;
}
