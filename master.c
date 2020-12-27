#include "sem_lib.h"
#include "utility.h"
#include "handling.h"

/* Prototipi di funzioni */
int create_matrix();
void fill_resource();

/* Variabili globali */
unsigned int SO_HOLES = 10;
unsigned int SO_SOURCES = 20;
unsigned int SO_CAP_MIN = 1;
unsigned int SO_CAP_MAX = 10;
unsigned int SO_TAXI = 95;
/* unsigned int SO_TOP_CELLS; */
unsigned long int SO_TIMENSEC_MIN = 100000000;
unsigned long int SO_TIMENSEC_MAX = 300000000;
unsigned int SO_TIMEOUT = 1;
unsigned int SO_DURATION = 20;
int request_flag = 0;

/* ID dell'IPC del semaforo e` globale */
int id_sem_cap, id_sem_taxi, id_sem_request;

struct shared_map *city;

int main(int argc, char const *argv[])
{
    /* Dichiarazione variabili */
    int id_shd_mem, id_msg_queue, cond, i, random_x_p, random_y_p, random_x_a, random_y_a, random_request;
    sigset_t my_mask;
    struct msg_request request;
    struct sigaction sa;

    /* Creazione signal header */
    bzero(&sa, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    sigaction(SIGALRM, &sa, NULL);

    /* Creazione memoria condivisa */
    id_shd_mem = shmget(IPC_PRIVATE, SHARED_MAP_LENGTH, IPC_CREAT | IPC_EXCL | 0666);
    TEST_ERROR;

    /* Attacching memoria condivisa */
    city = shmat(id_shd_mem, NULL, 0);
    TEST_ERROR;

    /* Creazione array semafori capienza */
    id_sem_cap = semget(IPC_PRIVATE, NUM_RISORSE, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR

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
    set_sem(id_sem_taxi, 0, 1);
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
    print_resource(id_sem_cap);

    printf("Premi INVIO per continuare.\n");
    getchar();
    printf("-----------------Creazione Richieste-----------------\n");
    /* Creazione richieste */
    for (i = 0; i < SO_SOURCES; i++)
    {
        switch (fork())
        {
        case -1:
            TEST_ERROR
            break;

        case 0:
            /* Creazione signal header */
            bzero(&sa, sizeof(struct sigaction));
            sa.sa_handler = request_handler;
            sigaction(SIGALRM, &sa, NULL);

            srand(getpid());

            while (1)
            {
                random_request = rand() % 5 + 1;
                alarm(random_request);

                sigemptyset(&my_mask);
                sigfillset(&my_mask);
                sigdelset(&my_mask, SIGALRM);
                sigsuspend(&my_mask);
                errno = 0;

                /* Estraggo coordinate partenza */
                do
                {
                    random_x_p = rand() % SO_HEIGHT;
                    random_y_p = rand() % SO_WIDTH;
                } while (city->matrix[random_x_p][random_y_p].is_hole && city->matrix[random_x_p][random_y_p].request_pid != 0);

                city->matrix[random_x_p][random_y_p].request_pid = getpid();

                /* Estraggo coordinate arrivo */
                do
                {
                    random_x_a = rand() % SO_HEIGHT;
                    random_y_a = rand() % SO_WIDTH;
                } while (city->matrix[random_x_a][random_y_a].is_hole && random_x_a == random_x_p && random_y_a == random_y_p);

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

                msgsnd(id_msg_queue, &request, REQUEST_LENGTH, 0);
                TEST_ERROR

                rel_sem(id_sem_request, INDEX(request.start.x, request.start.y));

                wait_sem_zero(id_sem_request, INDEX(request.start.x, request.start.y));
            }

            exit(EXIT_FAILURE);

        default:
            break;
        }
    }
    sleep(2);
    print_matrix(city, 5);

    getchar();
    printf("-----------------Creazione Taxi-----------------\n");
    /* Creazione taxi */
    for (i = 0; i < 1; i++)
    {
        switch (fork())
        {
        case -1:
            TEST_ERROR
            break;

        case 0:
        {
            char sid_shd_mem[20],
                sid_msg_queue[20],
                sid_sem_cap[20],
                sid_sem_taxi[20],
                sid_sem_request[20];

            snprintf(sid_shd_mem, 20, "%d", id_shd_mem);
            snprintf(sid_msg_queue, 20, "%d", id_msg_queue);
            snprintf(sid_sem_cap, 20, "%d", id_sem_cap);
            snprintf(sid_sem_taxi, 20, "%d", id_sem_taxi);
            snprintf(sid_sem_request, 20, "%d", id_sem_request);

            execlp(FILEPATH, FILEPATH, sid_shd_mem, sid_msg_queue, sid_sem_cap, sid_sem_taxi, sid_sem_request, NULL);
            TEST_ERROR

            exit(EXIT_FAILURE);
        }
        default:
            break;
        }
    }

    sleep(2);
    printf("Premi INVIO per continuare.\n");
    getchar();
    printf("-----------------Inizio Gioco-----------------\n");

    /* Semaforo wait for zero */
    printf("Master PID:%d : Rilascio il semaforo per i taxi...\n", getpid());
    set_sem(id_sem_taxi, 0, 0);
    TEST_ERROR

    /* Parte il timer SO_DURATION */
    printf("Master PID:%d : Timer gioco partito - %d sec.\n", getpid(), SO_DURATION);
    alarm(SO_DURATION);

    /* attesa figli? */
    sleep(18);
    printf("Master PID:%d : Elimino tutti gli IPC\n", getpid());

    /* Detaching ed eliminazione memoria condivisa */
    shmdt(city);
    shmctl(id_shd_mem, IPC_RMID, 0);
    TEST_ERROR

    /* Eliminazione semafori */
    semctl(id_sem_cap, 0, IPC_RMID);
    TEST_ERROR
    semctl(id_sem_taxi, 0, IPC_RMID);
    TEST_ERROR
    semctl(id_sem_request, 0, IPC_RMID);
    TEST_ERROR

    /* Chiusura coda di messaggi */
    msgctl(id_msg_queue, IPC_RMID, NULL);
    TEST_ERROR

    printf("Master PID:%d : Terminazione completata.\n", getpid());
    return 10;
}

int create_matrix()
{
    int i, j, z, i_holes, j_holes, hole_inseriti = 0, cond = 1, attempts = 0;
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
            if (attempts > 30)
                return -1; /* Fallimento */

            /* continue; */
        }
    }
    return 0;
}

void fill_resource()
{
    struct sembuf sops[NUM_RISORSE];
    int i, j;
    srand(getpid());

    for (i = 0; i < NUM_RISORSE; i++)
    {
        set_sem(id_sem_cap, i, rand() % SO_CAP_MAX + SO_CAP_MIN);
        TEST_ERROR
    }
}
