#include "utility.h"
#include "sem_lib.h"

/* Prototipi funzioni */
int create_taxi();
void alarm_handler();
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
int doing_request = 0;      /* Indica se il taxi sta eseguendo una richiesta */

int main(int argc, char const *argv[])
{
    /* Dichiarazione variabili */
    struct msg_request request;
    struct sigaction sa;
    coordinate source_position;
    int iter, i, j, a, b, found;

    /* Controllo dei parametri ricevuti */
    if (argc != 6)
    {
        fprintf(stderr, "Taxi PID:%d : Numero di parametri errato.\n", getpid());
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
    sa.sa_handler = alarm_handler;
    sigaction(SIGALRM, &sa, NULL);

    /* Attacching memoria condivisa */
    city = shmat(id_shd_mem, NULL, 0);
    TEST_ERROR

    /* Creazione in posizione casuale */
    if (create_taxi() == -1)
        fprintf(stderr, "Taxi PID:%d : Impossibile effettuare la creazione.\n", getpid());

    printf("Taxi PID:%d : Buongiornissimo, sono stato creato in (%d,%d):\n", getpid(), actual_position.x, actual_position.y);

    /* Semaforo wait for zero */
    wait_sem_zero(id_sem_taxi, 0);

    while (1)
    {
        /* Prelievo richieste con coda */
        printf("Taxi PID:%d : Cerco una richiesta...\n", getpid());

        /* Verifica se la richiesta si trova nella cella attuale */
        if (dec_sem_nw(id_sem_request, INDEX(actual_position.x, actual_position.y)) != -1)
        {
            printf("Taxi : Richiesta trovata nel punto di origine\n");
            source_position.x = actual_position.x;
            source_position.y = actual_position.y;
        }
        else if (errno == EAGAIN)
        {
            errno = 0;

            /* Controlla nelle celle adiacenti */
            found = 0;
            for (iter = 1; iter < QUARTIERE && !found; iter++)
            {
                a = -iter;
                b = iter;

                for (i = a; i <= b && !found; i++)
                {
                    for (j = a; j <= b && !found; j++)
                    {
                        if (i == a || i == b || j == a || j == b)
                        {
                            if (actual_position.x + i >= 0 && actual_position.y + j >= 0 && actual_position.x + i < SO_HEIGHT && actual_position.y + j < SO_WIDTH)
                                if (city->matrix[actual_position.x + i][actual_position.y + j].is_hole == 0)
                                    if (dec_sem_nw(id_sem_request, INDEX(actual_position.x + i, actual_position.y + j)) != -1)
                                    {
                                        source_position.x = actual_position.x + i;
                                        source_position.y = actual_position.y + j;
                                        printf("Taxi : Richiesta trovata nel punto più vicino (%d, %d) con PID %d: \n", source_position.x, source_position.y, city->matrix[source_position.x][source_position.y].request_pid);
                                        found = 1;
                                        printf("[%d, %d]\n", i, j);
                                    }
                                    else
                                    {
                                        errno = 0;
                                    }
                        }
                    }
                }
            }

            if (!found)
            {
                sleep(2); /* attendo qualche secondino prima di controllare nuovamente la mappa delle richieste */
                continue;
            }
        }
        errno = 0;
        TEST_ERROR
        msgrcv(id_msg_queue, &request, REQUEST_LENGTH, city->matrix[source_position.x][source_position.y].request_pid, 0);
        TEST_ERROR;

        printf("Taxi PID:%d : Richiesta trovata: \n"
               "- Partenza x : %d\n"
               "- Partenza y : %d \n"
               "- Arrivo x : %d \n"
               "- Arrivo y : %d\n",
               getpid(), request.start.x, request.start.y, request.end.x, request.end.y);
        doing_request = 1;

        /* Parte il timer SO_TIMEOUT */
        alarm(10); /* SO_TIMEOUT */

        /* Spostamento */
        move();
    }

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
        {
            dec_sem_nw(id_sem_cap, INDEX(random_x, random_y));
            attempts++;
        }

        if (attempts == 30)
            return -1;

    } while (errno == EAGAIN || city->matrix[random_x][random_y].is_hole);

    errno = 0;

    /* Aggiorno le posizioni attuali del taxi */
    actual_position.x = random_x;
    actual_position.y = random_y;

    /* Aggiornamento campi della matrice condivisa */
    city->matrix[random_x][random_y].crossing_cont++;

    return 0;
}

void alarm_handler(int signum)
{
    printf("I'm signal handler\n");
    if (signum == SIGALRM)
    {
        printf("Taxi PID:%d : Timer scaduto...\n", getpid());
        /* - gestire caso in cui stava gestendo una richiesta
         * - gestire rilascio di risorse (semafori)
         */
        rel_sem(id_sem_cap, INDEX(actual_position.x, actual_position.y));

        if (doing_request)
        {
            /* si comunica che la richiesta è stata abortita */
        }

        /* Detaching memoria condivisa */
        shmdt(city);
        TEST_ERROR
        exit(EXIT_SUCCESS);
    }
}

int move()
{
    /* usare move_up/down/left/right per arrivare alla destinazione :) 
     * https://rosettacode.org/wiki/A*_search_algorithm#C
     */

    sleep(1);
    doing_request = 0; /* richiesta completata */
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
