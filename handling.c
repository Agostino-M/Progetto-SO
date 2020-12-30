#include "handling.h"
#include <unistd.h>

void signal_handler(int signum)
{
    switch (signum)
    {
    case SIGALRM:
        printf("Master : Timer scaduto.. Il gioco termina.\n");
        break;
    case SIGUSR1:
        printf("Master : Segnale SIGUSR1 arrivato..\n");
        /* Crea una richiesta a mano / Richiama una funzione */
    }
}


void source_handler(int signum)
{
    switch (signum)
    {
        case SIGTERM:
            printf("Source PID:%d SIGTERM rievuto...\n", getpid());
            exit(EXIT_SUCCESS);
    }
}
