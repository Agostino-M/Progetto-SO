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

void request_handler(int signum)
{
    /* printf("Source PID:%d : segnale SIGALRM ricevuto\n", getpid()); */
}


