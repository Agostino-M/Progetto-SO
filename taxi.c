#include "utility.h"
#include "sem_lib.h"

/* Prototipi funzioni */
void alarm_handler();
int create_taxi();
void close_taxi();
void mutex_op(int op);
void move(int x, int y);
void move_up();
void move_down();
void move_left();
void move_right();

/* ID IPC globali */
int id_shd_mem, id_shd_stats, id_msg_queue, id_sem_cap, id_sem_taxi, id_sem_request, id_sem_stats;
int strada_fatta = 0, num_richieste = 0;
long durata_viaggio;
int SO_TIMEOUT;

struct shared_map *city;
struct shared_stats *stats;
struct timespec crossing_time;
struct timespec wait_a_moment;
coordinate actual_position;

int doing_request = 0; /* Indica se il taxi sta eseguendo una richiesta */

int main(int argc, char const *argv[])
{
    /* Dichiarazione variabili */
    struct msg_request request;
    struct sigaction sa;
    coordinate source_position;
    int iter, i, j, a, b, found;
    FILE *fp;
    wait_a_moment.tv_nsec = 300000000;

    if ((fp = fopen(INPUT_FILENAME, "r")) == NULL)
    {
        fprintf(stderr, ANSI_COLOR_RED "Taxi PID:%d : Errore nell'apertura del file, %d, %s" ANSI_COLOR_RESET, getpid(), errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%*s = %d", &SO_TIMEOUT);

    if (fclose(fp))
    {
        fprintf(stderr, ANSI_COLOR_RED "Taxi PID:%d : Errore nell'apertura del file, %d, %s" ANSI_COLOR_RESET, getpid(), errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Array di semafori di mutua esclusione per incremento contatori in mem condivisa
     * sem[0] : Eseguiti con successo
     * sem[1] : Abortiti
     * sem[2] : Taxi che ha fatto più strada
     * sem[3] : Taxi che ha fatto il viaggio più lungo
     * sem[4] : Taxi che ha raccolto più richieste
     */

    /* Controllo dei parametri ricevuti */
    if (argc != 8)
    {
        fprintf(stderr, ANSI_COLOR_RED "Taxi PID:%d : Numero di parametri errato.\n" ANSI_COLOR_RESET, getpid());
        exit(EXIT_FAILURE);
    }

    /* 
     * Prelievo argomenti argv 
     * - argv[1] = id memoria condivisa city
     * - argv[2] = id memoria condivisa stats
     * - argv[3] = id coda di messaggi per le richieste
     * - argv[4] = id semaforo delle capacità massime per taxi su una cella
     * - argv[5] = id semaforo per accedere in mutua escl alla mem condivisa stats
     * - argv[6] = id semaforo "wait for zero" che dà il via ai taxi
     * - argv[7] = id semaforo che indica la presenza di una richiesta
     */
    id_shd_mem = atoi(argv[1]);
    id_shd_stats = atoi(argv[2]);
    id_msg_queue = atoi(argv[3]);
    id_sem_cap = atoi(argv[4]);
    id_sem_stats = atoi(argv[5]);
    id_sem_taxi = atoi(argv[6]);
    id_sem_request = atoi(argv[7]);

    /* Creazione signal header */
    bzero(&sa, sizeof(struct sigaction));
    sa.sa_handler = alarm_handler;
    sigaction(SIGALRM, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    TEST_ERROR

    /* Attacching memoria condivisa */
    city = shmat(id_shd_mem, NULL, 0);
    TEST_ERROR
    stats = shmat(id_shd_stats, NULL, 0);
    TEST_ERROR

    /* Creazione in posizione casuale */
    if (create_taxi() == -1)
    {
        fprintf(stderr, ANSI_COLOR_RED "Taxi PID:%d : Impossibile effettuare la creazione.\n" ANSI_COLOR_RESET, getpid());
        exit(EXIT_FAILURE);
    }

    /*Taxi attende che tutti gli altri siano creati e inizializzati*/
    dec_sem(id_sem_taxi, 0);
    TEST_ERROR
    wait_sem_zero(id_sem_taxi, 0);
    TEST_ERROR

    while (1)
    {
        /* Parte il timer SO_TIMEOUT */
        alarm(SO_TIMEOUT);

        /* Verifica se la richiesta si trova nella cella attuale */
        if (dec_sem_nw(id_sem_request, INDEX(actual_position.x, actual_position.y)) != -1)
        {
            source_position.x = actual_position.x;
            source_position.y = actual_position.y;
            doing_request = 1;
        }
        else
        {
            if (errno == EAGAIN)
                errno = 0;
            TEST_ERROR

            /* Controlla nelle celle adiacenti */
            found = 0;
            for (iter = 1; iter <= QUARTIERE && !found; iter++)
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
                                {
                                    if (dec_sem_nw(id_sem_request, INDEX(actual_position.x + i, actual_position.y + j)) != -1)
                                    {

                                        source_position.x = actual_position.x + i;
                                        source_position.y = actual_position.y + j;
                                        found = 1;
                                        doing_request = 1;
                                    }
                                    else
                                    {
                                        if (errno == EAGAIN)
                                            errno = 0;
                                        TEST_ERROR
                                    }
                                }
                        }
                    }
                }
            }

            if (!found)
            {
                /* Attendo qualche nanosecondo prima di ricontrollare la mappa delle richieste */
                nanosleep(&wait_a_moment, NULL);
                continue;
            }

            /* Si sposta verso la richiesta più vicina trovata */
            else
                move(source_position.x, source_position.y);
        }

        alarm(0);

        msgrcv(id_msg_queue, &request, REQUEST_LENGTH, city->matrix[source_position.x][source_position.y].request_pid, 0);
        TEST_ERROR;

        num_richieste++;

        /* Spostamento verso la destinazione */
        durata_viaggio = 0;
        move(request.end.x, request.end.y);

        doing_request = 0; /* Richiesta completata */

        /* Segnalo che ho completato la richiesta */
        mutex_op(0);
    }

    close_taxi();
}

void alarm_handler(int signum)
{
    if (signum == SIGTERM)
    {
        alarm(0);
        close_taxi();
    }

    else if (signum == SIGALRM)
    {
        close_taxi();
    }
}

int create_taxi()
{
    int random_x, random_y, attempts = 0;

    srand(getpid());

    do
    {
        if (errno == EAGAIN)
            errno = 0;
        TEST_ERROR

        random_x = rand() % SO_HEIGHT;
        random_y = rand() % SO_WIDTH;

        if (!city->matrix[random_x][random_y].is_hole)
        {
            dec_sem_nw(id_sem_cap, INDEX(random_x, random_y));
            attempts++;
        }

        if (attempts == 50)
            return -1;

    } while (errno == EAGAIN || city->matrix[random_x][random_y].is_hole);
    TEST_ERROR

    /* Aggiorno le posizioni attuali del taxi */
    actual_position.x = random_x;
    actual_position.y = random_y;

    /* Aggiornamento campi della matrice condivisa */
    city->matrix[random_x][random_y].crossing_cont++;

    return 0;
}

void close_taxi()
{
    int status = getpid();

    rel_sem(id_sem_cap, INDEX(actual_position.x, actual_position.y)); /* Libera la cella su cui si trovava */
    TEST_ERROR

    if (doing_request)
        mutex_op(1); /* Viaggio abortito */

    /* Detaching memoria condivisa */
    shmdt(city);
    TEST_ERROR
    shmdt(stats);
    TEST_ERROR
    exit(status);
}

void mutex_op(int op)
{
    if (op > 1 || op < 0)
        return;

    dec_sem(id_sem_stats, op);
    TEST_ERROR

    switch (op)
    {
    case 0: /* Viaggio eseguito */
        stats->num_viaggi_eseguiti++;

        if (stats->max_strada_fatta < strada_fatta) /* Taxi max strada fatta */
        {
            stats->max_strada_fatta = strada_fatta;
            stats->pid_max_strada_fatta = getpid();
        }

        if (stats->max_viaggio < durata_viaggio) /* Taxi max viaggi */
        {
            stats->max_viaggio = durata_viaggio;
            stats->pid_max_viaggio = getpid();
        }

        if (stats->max_richieste < num_richieste) /* Taxi max richieste eseguite */
        {
            stats->max_richieste = num_richieste;
            stats->pid_max_richieste = getpid();
        }

        break;

    case 1: /* Viaggio abortito */
        stats->num_viaggi_abortiti++;
        break;
    }

    rel_sem(id_sem_stats, op);
    TEST_ERROR
}

void move(int x, int y)
{
    int Dx, Dy;

    Dx = x - actual_position.x;
    Dy = y - actual_position.y;

    while (Dx != 0)
    {
        Dy = y - actual_position.y;

        if (Dx > 0) /* Mi muovo verso il basso */
        {
            if (city->matrix[actual_position.x + 1][actual_position.y].is_hole) /* hole */
            {
                if (actual_position.y == SO_WIDTH - 1 || Dy < 0)
                {
                    move_left();
                    move_down();
                    Dx--;
                }
                else
                {
                    move_right();
                    move_down();
                    Dx--;
                }
            }

            else
            {
                move_down();
                Dx--;
            }
        }

        else /* Mi muovo verso l'alto Dx < 0 */
        {
            if (city->matrix[actual_position.x - 1][actual_position.y].is_hole) /* hole */
            {
                if (actual_position.y == SO_WIDTH - 1 || Dy < 0)
                {

                    move_left();
                    move_up();
                    Dx++;
                }
                else
                {
                    move_right();
                    move_up();
                    Dx++;
                }
            }

            else
            {
                move_up();
                Dx++;
            }
        }
    }

    /* Mi sono spostato, Dy è cambiato */
    Dy = y - actual_position.y;

    while (Dy != 0)
    {
        if (Dy < 0) /* Mi muovo a sx */
        {

            if (city->matrix[actual_position.x][actual_position.y - 1].is_hole) /* hole */
            {
                if (actual_position.x == SO_HEIGHT - 1)
                {
                    move_up();
                    move_left();
                    move_left();
                    move_down();
                    Dy = Dy + 2;
                }
                else
                {
                    move_down();
                    move_left();
                    move_left();
                    move_up();
                    Dy = Dy + 2;
                }
            }

            else
            {
                move_left();
                Dy++;
            }
        }

        else
        {
            if (city->matrix[actual_position.x][actual_position.y + 1].is_hole) /* hole */
            {
                if (actual_position.x == SO_HEIGHT - 1)
                {
                    move_up();
                    move_right();
                    move_right();
                    move_down();
                    Dy = Dy - 2;
                }
                else
                {
                    move_down();
                    move_right();
                    move_right();
                    move_up();
                    Dy = Dy - 2;
                }
            }

            else
            {
                move_right();
                Dy--;
            }
        }
    }
}

void move_up()
{
    dec_sem_wait(id_sem_cap, INDEX(actual_position.x - 1, actual_position.y), SO_TIMEOUT);

    if (errno == EAGAIN) /* Scaduto SO_TIMEOUT */
    {
        errno = 0;
        close_taxi();
    }
    TEST_ERROR

    actual_position.x--;
    rel_sem(id_sem_cap, INDEX(actual_position.x + 1, actual_position.y));
    TEST_ERROR
    city->matrix[actual_position.x][actual_position.y].crossing_cont++;
    crossing_time.tv_nsec = city->matrix[actual_position.x][actual_position.y].crossing_time;
    durata_viaggio += crossing_time.tv_nsec;
    strada_fatta++;
    nanosleep(&crossing_time, NULL);
}

void move_down()
{
    dec_sem_wait(id_sem_cap, INDEX(actual_position.x + 1, actual_position.y), SO_TIMEOUT);
    if (errno == EAGAIN) /* Scaduto SO_TIMEOUT */
    {
        errno = 0;
        close_taxi();
    }
    TEST_ERROR

    actual_position.x++;
    rel_sem(id_sem_cap, INDEX(actual_position.x - 1, actual_position.y));
    TEST_ERROR
    city->matrix[actual_position.x][actual_position.y].crossing_cont++;
    crossing_time.tv_nsec = city->matrix[actual_position.x][actual_position.y].crossing_time;
    durata_viaggio += crossing_time.tv_nsec;
    strada_fatta++;
    nanosleep(&crossing_time, NULL);
}

void move_left()
{
    dec_sem_wait(id_sem_cap, INDEX(actual_position.x, actual_position.y - 1), SO_TIMEOUT);
    if (errno == EAGAIN) /* Scaduto SO_TIMEOUT */
    {
        errno = 0;
        close_taxi();
    }
    TEST_ERROR

    actual_position.y--;
    rel_sem(id_sem_cap, INDEX(actual_position.x, actual_position.y + 1));
    TEST_ERROR
    city->matrix[actual_position.x][actual_position.y].crossing_cont++;
    crossing_time.tv_nsec = city->matrix[actual_position.x][actual_position.y].crossing_time;
    durata_viaggio += crossing_time.tv_nsec;
    strada_fatta++;
    nanosleep(&crossing_time, NULL);
}

void move_right()
{
    dec_sem_wait(id_sem_cap, INDEX(actual_position.x, actual_position.y + 1), SO_TIMEOUT);
    if (errno == EAGAIN) /* Scaduto SO_TIMEOUT */
    {
        errno = 0;
        close_taxi();
    }
    TEST_ERROR

    actual_position.y++;
    rel_sem(id_sem_cap, INDEX(actual_position.x, actual_position.y - 1));
    TEST_ERROR
    city->matrix[actual_position.x][actual_position.y].crossing_cont++;
    crossing_time.tv_nsec = city->matrix[actual_position.x][actual_position.y].crossing_time;
    durata_viaggio += crossing_time.tv_nsec;
    strada_fatta++;
    nanosleep(&crossing_time, NULL);
}