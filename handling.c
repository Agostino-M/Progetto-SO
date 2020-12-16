#include "handling.h"

void signal_handler(int signum)
{
    switch (signum)
    {
    case SIGINT:
        printf("Master PID:%d: Segnale SIGINT arrivato.. non posso gestirlo.\n");
        break;
    case SIGALRM:
        printf("Master PID%d: Timer scaduto.. Il gioco termina.\n");
        /*PRINT_METRIC*/
        kill(0, SIGTERM);
        exit(EXIT_SUCCESS);
        break;
    case SIGUSR1:
        printf("Master PID%d: Segnale SIGUSR1 arrivato..\n");
        /* Crea una richiesta a mano / Richiama una funzione */
    }
}