#include "sem_lib.h"
#include "utility.h"
#include "sem_list.h"
/*#include "handling.h"/*

/* Prototipi di funzioni */
int create_matrix();
void fill_resource();
void signal_handler(int signum);
void source_handler(int signum);
void print_handler(int signum);
void close_master();
void create_taxi_child();
void kill_all_child();
void print_top_cells();

/* Variabili globali */
int SO_HOLES;
int SO_SOURCES;
int SO_CAP_MIN;
int SO_CAP_MAX;
int SO_TAXI;
int SO_TOP_CELLS;
long SO_TIMENSEC_MIN;
long SO_TIMENSEC_MAX;
int SO_TIMEOUT;
int SO_DURATION;
int cont_taxi = 0;
int cont_sources = 0;
int flag_timer = 0; /* flag dell'handler del master*/
int stop_create = 0;
/* ID dell'IPC del semaforo e` globale */
int id_sem_cap, id_sem_taxi, id_sem_stats, id_sem_request, id_shd_mem, id_shd_stats, id_msg_queue, id_sem_write;
struct shared_map *city;
struct shared_stats *stats;
struct msg_request request;
lista_pid *taxi_pid = NULL;
lista_pid *sources_pid = NULL;
lista_pid *top_cells = NULL;

int main(int argc, char const *argv[])
{
    /* Dichiarazione variabili */
    int cond, i, random_x_p, random_y_p, random_x_a, random_y_a, random_request, fork_value, pid_print, status;
    sigset_t my_mask;
    struct sigaction sa;
    char *temp;
    FILE *fp;

    if ((fp = fopen(INPUT_FILENAME, "r")) == NULL)
    {
        fprintf(stderr, "Master PID:%d : Errore nell'apertura del file, %d, %s", getpid(), errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%*s = %d", &SO_TIMEOUT);
    printf("SO_TIMETOUT: %d\n", SO_TIMEOUT);

    fscanf(fp, "%*s = %d", &SO_HOLES);
    printf("SO_HOLES: %d\n", SO_HOLES);

    fscanf(fp, "%*s = %d", &SO_SOURCES);
    printf("SO_SOURCES: %d\n", SO_SOURCES);

    fscanf(fp, "%*s = %d", &SO_CAP_MIN);
    printf("SO_CAP_MIN: %d\n", SO_CAP_MIN);

    fscanf(fp, "%*s = %d", &SO_CAP_MAX);
    printf("SO_CAP_MAX: %d\n", SO_CAP_MAX);

    fscanf(fp, "%*s = %d", &SO_TAXI);
    printf("SO_TAXI: %d\n", SO_TAXI);

    fscanf(fp, "%*s = %d", &SO_TOP_CELLS);
    printf("SO_TOP_CELLS: %d\n", SO_TOP_CELLS);

    fscanf(fp, "%*s = %ld", &SO_TIMENSEC_MIN);
    printf("SO_TIMENSEC_MIN: %ld\n", SO_TIMENSEC_MIN);

    fscanf(fp, "%*s = %ld", &SO_TIMENSEC_MAX);
    printf("SO_TIMENSEC_MAX: %ld\n", SO_TIMENSEC_MAX);

    fscanf(fp, "%*s = %d", &SO_DURATION);
    printf("SO_DURATION: %d\n", SO_DURATION);

    if (fclose(fp))
        fprintf(stderr, "Master PID:%d : Errore nell'apertura del file, %d, %s", getpid(), errno, strerror(errno));

    printf("Master PID:%d : Inizializzazione gioco...\n", getpid());

    /* Creazione signal handler */
    bzero(&sa, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    /*sa.sa_flags = SA_NODEFER;*/
    sigaction(SIGALRM, &sa, NULL);
    /*sigaction(SIGUSR1, &sa, NULL);*/
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    /* Creazione memoria condivisa */
    id_shd_mem = shmget(IPC_PRIVATE, SHARED_MAP_LENGTH, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR;
    id_shd_stats = shmget(IPC_PRIVATE, SHARED_STATS_LENGHT, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR;

    /* Attacching memoria condivisa */
    city = shmat(id_shd_mem, NULL, 0);
    TEST_ERROR;
    stats = shmat(id_shd_stats, NULL, 0);
    TEST_ERROR

    /* Creazione array semafori capienza */
    id_sem_cap = semget(IPC_PRIVATE, NUM_RISORSE, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR

    /* Creazione array semafori per accede in mutua escl ai contatori in mem condivisa */
    id_sem_stats = semget(IPC_PRIVATE, 5, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR
    for (i = 0; i < 5; i++)
    {
        set_sem(id_sem_stats, i, 1);
        TEST_ERROR
    }

    /* Creazione semaforo scrittura su coda mutex*/
    id_sem_write = semget(IPC_PRIVATE, NUM_RISORSE, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR
    for (i = 0; i < NUM_RISORSE; i++)
    {
        set_sem(id_sem_write, i, 1);
        TEST_ERROR
    }

    /* Creazione semaforo  richieste */
    id_sem_request = semget(IPC_PRIVATE, NUM_RISORSE, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR
    for (i = 0; i < NUM_RISORSE; i++)
    {
        set_sem(id_sem_request, i, 0);
        TEST_ERROR
    }

    /* Creazione semaforo "wait for zero" taxi */
    id_sem_taxi = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR
    set_sem(id_sem_taxi, 0, SO_TAXI + 1);
    TEST_ERROR

    /* Creazione coda di messaggi */
    id_msg_queue = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR

    /* Creazione matrice */
    if (SO_HOLES > (ceil(SO_HEIGHT / 2.0) * ceil(SO_WIDTH / 2.0)))
    {
        fprintf(stderr, "Master : impossibile creare la matrice con questi parametri\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        do
        {
            cond = create_matrix();
        } while (cond != 0);
    }

    print_matrix(city, 3);

    /* Inizializzazione del vettore di semafori */
    fill_resource();
    /*print_resource(id_sem_cap);*/

    printf("Premi INVIO per continuare.\n");
    getchar();
    printf(ANSI_COLOR_GREEN "---------------------Creazione Richieste---------------------\n" ANSI_COLOR_RESET);

    /* Creazione richieste */
    for (i = 0; i < SO_SOURCES; i++)
    {
        switch (fork_value = fork())
        {
        case -1:
            TEST_ERROR
            break;

        case 0:
        {
            struct sembuf sops[2];

            /* Creazione source handler */
            bzero(&sa, sizeof(struct sigaction));
            sa.sa_handler = source_handler;
            sigaction(SIGALRM, &sa, NULL);
            sigaction(SIGTERM, &sa, NULL);
            sigaction(SIGUSR2, &sa, NULL);
            srand(getpid());

            printf("Source PID:%d \n", getpid());

            while (1)
            {
                random_request = rand() % 5 + 1;
                alarm(random_request);

                sigemptyset(&my_mask);
                sigfillset(&my_mask);
                sigdelset(&my_mask, SIGALRM);
                sigdelset(&my_mask, SIGTERM);
                sigsuspend(&my_mask);

                /* Gestiamo il valore di ritorno EINTR*/
                if (errno == EINTR)
                {
                    errno = 0;
                }
                TEST_ERROR

                do
                {
                    if (errno == EAGAIN)
                        errno = 0;
                    TEST_ERROR
                    random_x_p = rand() % SO_HEIGHT;
                    random_y_p = rand() % SO_WIDTH;

                    if (!city->matrix[random_x_p][random_y_p].is_hole)
                    {
                        /* Semaforo di mutua esclusione per la scrittura su coda */
                        dec_sem_nw(id_sem_write, INDEX(random_x_p, random_y_p));
                    }

                } while (errno == EAGAIN || city->matrix[random_x_p][random_y_p].is_hole || semctl(id_sem_request, INDEX(random_x_p, random_y_p), GETVAL) != 0);
                TEST_ERROR

                city->matrix[random_x_p][random_y_p].request_pid = getpid();

                /* Estraggo coordinate arrivo */
                do
                {
                    random_x_a = rand() % SO_HEIGHT;
                    random_y_a = rand() % SO_WIDTH;
                } while (city->matrix[random_x_a][random_y_a].is_hole || random_x_a == random_x_p && random_y_a == random_y_p);

                /* Write del messaggio sulla coda */
                request.mtype = (long)getpid();
                request.start.x = random_x_p;
                request.start.y = random_y_p;
                request.end.x = random_x_a;
                request.end.y = random_y_a;

                /*printf("Source PID:%d : Nuova richiesta creata: \n"
                       "- Partenza x : %d\n"
                       "- Partenza y : %d \n"
                       "- Arrivo x : %d \n"
                       "- Arrivo y : %d\n",
                       getpid(), request.start.x, request.start.y, request.end.x, request.end.y);*/

                msgsnd(id_msg_queue, &request, REQUEST_LENGTH, 0);
                TEST_ERROR

                rel_sem(id_sem_request, INDEX(request.start.x, request.start.y));
                TEST_ERROR

                do
                {
                    if (errno == EINTR)
                        errno = 0;
                    TEST_ERROR
                    wait_sem_zero(id_sem_request, INDEX(request.start.x, request.start.y));
                } while (errno == EINTR); 


                /*Fine sezione critica*/
                rel_sem(id_sem_write, INDEX(request.start.x, request.start.y));
                TEST_ERROR
            }

            exit(EXIT_FAILURE);
        }
        default:
            sources_pid = insert_pid(sources_pid, fork_value);
            TEST_ERROR
            break;
        }
    }

    sleep(6); /*sleep per visualizzare l'array delle richieste completo*/
    printf("\nArray delle richieste\n\n");
    /*print_resource(id_sem_request);*/
    print_matrix(city, 5);
    printf("Premi INVIO per continuare.\n");
    getchar();

    printf("---------------------Creazione Taxi---------------------\n");
    /* Creazione taxi */
    for (i = 0; i < SO_TAXI; i++)
    {
        switch (fork_value = fork())
        {
        case -1:
            TEST_ERROR
            break;

        case 0:
            printf("Taxi PID:%d\n", getpid());
            create_taxi_child();
            break;

        default:
            taxi_pid = insert_pid(taxi_pid, fork_value);
            TEST_ERROR
            break;
        }
    }

    /*sleep(2);*/
    /*printf("Premi INVIO per continuare.\n");
    getchar();*/

    sleep(2); /*Per mostrare la printf non sovrapposta */
    printf(ANSI_COLOR_GREEN "---------------------Inizio Gioco---------------------\n" ANSI_COLOR_RESET);

    /* Semaforo wait for zero */
    printf("\nMaster PID:%d : Rilascio il semaforo per i taxi...\n", getpid());
    /*set_sem(id_sem_taxi, 0, 0);*/
    dec_sem(id_sem_taxi, 0);
    wait_sem_zero(id_sem_taxi, 0);
    TEST_ERROR

    /* Parte il timer SO_DURATION */
    printf("Master PID:%d : Timer gioco partito - %d sec.\n", getpid(), SO_DURATION);
    alarm(SO_DURATION);

    /*Processo per stampa ogni secondo*/
    switch (pid_print = fork())
    {
    case -1:
        TEST_ERROR
        break;

    case 0:
        bzero(&sa, sizeof(struct sigaction));
        sa.sa_handler = print_handler;
        sigaction(SIGTERM, &sa, NULL);
        while (1)
        {
            printf("Mappa della città:\n");
            print_status(city, id_sem_cap);
            sleep(1);
        }
        if (errno == EINTR)
            errno = 0;

        TEST_ERROR
        break;
    default:
        TEST_ERROR
        break;
    }

    TEST_ERROR

    /*Aspetto che finisce SO_DURATION*/
    while ((waitpid(-1, &status, 0))&& stop_create == 0)
    {
        if(WIFEXITED(status))
        {
            rel_sem(id_sem_taxi, 0);
            switch (fork_value = fork())
            {
            case -1:
                TEST_ERROR
                break;

            case 0:
                create_taxi_child();
                break;
            
            default:
                TEST_ERROR
                taxi_pid = insert_pid(taxi_pid, fork_value);
                break;
            }
        }
    }

    if (stop_create == 1)
    {
        kill(pid_print, SIGTERM);
    }

    printf("Master : Timer scaduto.. Il gioco termina.\n");
    close_master();
}

void create_taxi_child()
{
    char sid_shd_mem[20],
        sid_shd_stats[20],
        sid_msg_queue[20],
        sid_sem_cap[20],
        sid_sem_stats[20],
        sid_sem_taxi[20],
        sid_sem_request[20];

    snprintf(sid_shd_mem, 20, "%d", id_shd_mem);
    snprintf(sid_shd_stats, 20, "%d", id_shd_stats);
    snprintf(sid_msg_queue, 20, "%d", id_msg_queue);
    snprintf(sid_sem_cap, 20, "%d", id_sem_cap);
    snprintf(sid_sem_stats, 20, "%d", id_sem_stats);
    snprintf(sid_sem_taxi, 20, "%d", id_sem_taxi);
    snprintf(sid_sem_request, 20, "%d", id_sem_request);

    execlp(FILEPATH, FILEPATH, sid_shd_mem, sid_shd_stats, sid_msg_queue, sid_sem_cap, sid_sem_stats, sid_sem_taxi, sid_sem_request, NULL);
    TEST_ERROR

    exit(EXIT_FAILURE);
}

void close_master()
{
    struct msqid_ds buff;
    int viaggi_inevasi;

    kill_all_child();

    /* Attesa terminazione figli */
    do
    {

        while (wait(NULL) != -1)
            ;
    } while (errno == EINTR && errno != ECHILD);

    if (errno == ECHILD)
        errno = 0;
    TEST_ERROR

    /* Stampa carattersitiche finali  */

    /*printf("Matrice con le richieste :\n");*/
    /*print_resource(id_sem_request);*/

    printf("SO_TOP_CELLS %d celle più attraversate :\n", SO_TOP_CELLS);

    print_top_cells();

    /* Prelievo viaggi inevasi */
    msgctl(id_msg_queue, IPC_STAT, &buff);
    viaggi_inevasi = buff.msg_qnum;

    printf("Viaggi eseguiti : %d\n"
           "Viaggi abortiti : %d\n"
           "Viaggi inevasi : %d\n\n",
           stats->num_viaggi_eseguiti, stats->num_viaggi_abortiti, viaggi_inevasi);

    printf("Il TAXI %d ha fatto più strada : %d\n"
           "Il TAXI %d ha fatto il viaggio più lungo : %ld\n"
           "Il TAXI %d ha raccolto più richieste : %d\n\n",
           stats->pid_max_strada_fatta, stats->max_strada_fatta,
           stats->pid_max_viaggio, stats->max_viaggio,
           stats->pid_max_richieste, stats->max_richieste);

    /*Eliminazione IPC */
    printf("Master PID:%d : Elimino tutti gli IPC\n", getpid());

    /* Detaching ed eliminazione memoria condivisa */
    shmdt(city);
    shmctl(id_shd_mem, IPC_RMID, 0);
    TEST_ERROR
    shmdt(stats);
    shmctl(id_shd_stats, IPC_RMID, 0);
    TEST_ERROR

    /* Eliminazione semafori */
    semctl(id_sem_cap, 0, IPC_RMID);
    TEST_ERROR
    semctl(id_sem_stats, 0, IPC_RMID);
    TEST_ERROR
    semctl(id_sem_taxi, 0, IPC_RMID);
    TEST_ERROR
    semctl(id_sem_request, 0, IPC_RMID);
    TEST_ERROR
    semctl(id_sem_write, 0, IPC_RMID);
    TEST_ERROR

    /* Chiusura coda di messaggi */
    msgctl(id_msg_queue, IPC_RMID, NULL);
    TEST_ERROR

    free(taxi_pid);
    free(sources_pid);
    free(top_cells);
    printf("Master PID:%d : Terminazione completata.\n", getpid());
    exit(EXIT_SUCCESS);
}

int create_matrix()
{
    int i, j, z, i_holes, j_holes, hole_inseriti = 0, cond = 1, attempts = 0;
    long int random;

    srand(getpid());

    for (i = 0; i < SO_HEIGHT; i++)
    {
        for (j = 0; j < SO_WIDTH; j++)
        {
            random = rand() % (SO_TIMENSEC_MAX - SO_TIMENSEC_MIN + 1) + SO_TIMENSEC_MIN;
            city->matrix[i][j].crossing_time = random;
            city->matrix[i][j].is_top = 0;
            city->matrix[i][j].is_hole = 0;
            city->matrix[i][j].crossing_cont = 0;
            city->matrix[i][j].request_pid = 0;
        }
    }

    srand(time(NULL));

    for (z = 0; z < SO_HOLES; z++)
    {
        cond = 1;
        i_holes = rand() % SO_HEIGHT;
        j_holes = rand() % SO_WIDTH;

        if (i_holes == 0) /* Riga i estratta è il bordo superiore (i=0) */
            i = i_holes;
        else
            i = i_holes - 1;

        for (i; i <= i_holes + 1 && cond == 1; i++)
        {
            if (j_holes == 0) /* Colonna j estratta è il bordo laterale sinistro (j=0)*/
                j = j_holes;
            else
                j = j_holes - 1;

            for (j; j <= j_holes + 1; j++)
                if (city->matrix[i][j].is_hole == 1)
                    cond = 0;
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
            attempts++;
            if (attempts > 90)
                return -1; /* Fallimento */
        }
    }
    return 0;
}

void fill_resource()
{
    struct sembuf sops[NUM_RISORSE];
    int i, j, random;
    srand(getpid());

    for (i = 0; i < SO_HEIGHT; i++)
    {
        for (j = 0; j < SO_WIDTH; j++)
        {
            if (city->matrix[i][j].is_hole)
            {
                set_sem(id_sem_cap, INDEX(i, j), 0);
                TEST_ERROR
            }
            else
            {
                random = rand() % (SO_CAP_MAX - SO_CAP_MIN + 1) + SO_CAP_MIN;
                set_sem(id_sem_cap, INDEX(i, j), random);
                TEST_ERROR
                city->matrix[i][j].nmax_taxi = random;
            }
        }
    }
}

void signal_handler(int signum)
{

    switch (signum)
    {

    case SIGINT:
        close_master();
        break;

    case SIGALRM:
        flag_timer = 1;
        stop_create = 1;
        break;
    }
}

void print_handler(int signum)
{
    switch (signum)
    {
    case SIGTERM:
        exit(EXIT_SUCCESS);
        break;
    }
}

void source_handler(int signum)
{
    switch (signum)
    {
    case SIGTERM:
        exit(EXIT_SUCCESS);

    case SIGUSR2:
    {
        int random_x_p, random_y_p, random_x_a, random_y_a;

        printf("SOURCE PID:%d  : Richiesta da terminale ricevuta\n", getpid());

        do
        {
            if (errno == EAGAIN)
                errno = 0;
            TEST_ERROR
            random_x_p = rand() % SO_HEIGHT;
            random_y_p = rand() % SO_WIDTH;

            if (!city->matrix[random_x_p][random_y_p].is_hole)
            {
                dec_sem_nw(id_sem_write, INDEX(random_x_p, random_y_p));
            }

        } while (errno == EAGAIN || city->matrix[random_x_p][random_y_p].is_hole || semctl(id_sem_request, INDEX(random_x_p, random_y_p), GETVAL) != 0);
        TEST_ERROR

        city->matrix[random_x_p][random_y_p].request_pid = getpid();

        /* Estraggo coordinate arrivo */
        do
        {
            random_x_a = rand() % SO_HEIGHT;
            random_y_a = rand() % SO_WIDTH;
        } while (city->matrix[random_x_a][random_y_a].is_hole || random_x_a == random_x_p && random_y_a == random_y_p);

        /* Write del messaggio sulla coda */
        request.mtype = (long)getpid();
        request.start.x = random_x_p;
        request.start.y = random_y_p;
        request.end.x = random_x_a;
        request.end.y = random_y_a;

        printf("Source PID:%d : Nuova richiesta creata: \n"
               "- Partenza x : %d\n"
               "- Partenza y : %d \n"
               "- Arrivo x : %d \n"
               "- Arrivo y : %d\n",
               getpid(), request.start.x, request.start.y, request.end.x, request.end.y);

        /* Semaforo di mutua esclusione per la scrittura su coda */
        msgsnd(id_msg_queue, &request, REQUEST_LENGTH, 0);
        TEST_ERROR

        rel_sem(id_sem_request, INDEX(request.start.x, request.start.y));
        TEST_ERROR

        rel_sem(id_sem_write, INDEX(request.start.x, request.start.y));
        TEST_ERROR

        /*print_resource(id);*/
    }
    }
}

void kill_all_child()
{
    lista_pid *p = taxi_pid, *q = sources_pid;

    while (q != NULL)
    {
        kill(q->pid, SIGTERM);
        q = q->next;
    }

    while (p != NULL)
    {
        kill(p->pid, SIGTERM);
        p = p->next;
    }
}

void print_top_cells()
{
    int i, j, z = 0;
    lista_pid *nodo_min, *q;

    /*Inserisco i primi SO_TOP_CELLS*/

    for (i = 0; i < SO_TOP_CELLS; i++)
    {
        top_cells = insert_attraversate(top_cells, -1);
    }

    for (i = 0; i < SO_HEIGHT; i++)
    {
        for (j = 0; j < SO_WIDTH; j++)
        {
            nodo_min = min_node(top_cells);
            if (nodo_min->attraversate < city->matrix[i][j].crossing_cont)
            {
                /*Elimino il nodo minimo e inserisco quello appena letto dalla matrice*/
                top_cells = delete_attraversate(top_cells, nodo_min->attraversate);
                top_cells = insert_attraversate(top_cells, city->matrix[i][j].crossing_cont);
                /*Salvo anche le coordinate del nuovo nodo di top_cells*/
                top_cells->posizione.x = i;
                top_cells->posizione.y = j;
            }
        }
    }

    while (top_cells != NULL)
    {
        city->matrix[top_cells->posizione.x][top_cells->posizione.y].is_top = 1;
        top_cells = top_cells->next;
    }

    printf("\n");

    for (i = 0; i < SO_HEIGHT; i++)
    {
        for (j = 0; j < SO_WIDTH; j++)
        {

            if (city->matrix[i][j].request_pid != 0)
            {
                if (city->matrix[i][j].is_top)
                {
                    printf(ANSI_COLOR_YELLOW "%2d " ANSI_COLOR_RESET, city->matrix[i][j].crossing_cont);
                }

                else
                {
                    printf(ANSI_COLOR_BLUE "%2d " ANSI_COLOR_RESET, city->matrix[i][j].crossing_cont);
                }
            }
            else
            {
                if (city->matrix[i][j].is_top)
                {
                    printf(ANSI_COLOR_BLUE " . " ANSI_COLOR_RESET);
                }
                else
                {
                    printf(" . ");
                }
            }
        }
        printf("\n");
    }

    printf("\n");
}