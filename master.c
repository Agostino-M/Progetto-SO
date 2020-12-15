#include "sem_lib.h"
#include "utility.h"

/* parametri definiti a tempo di compilazione */
#ifdef DENSE
#define SO_WIDTH 20
#define SO_HEIGHT 10
#elif LARGE
#define SO_WIDTH 60
#define SO_HEIGHT 20
#else
#define SO_WIDTH 10
#define SO_HEIGHT 10
#endif

/*Prototipi di funzioni*/
void creaMatrice();

unsigned int SO_HOLES;
unsigned int SO_TOP_CELLS;
unsigned int SO_SOURCES;
unsigned int SO_CAP_MIN;
unsigned int SO_CAP_MAX;
unsigned int SO_TAXI;
unsigned int SO_TOP_CELLS;
unsigned long int SO_TIMENSEC_MIN;
unsigned long int SO_TIMENSEC_MAX;
unsigned int SO_TIMEOUT;
unsigned int SO_DURATION;

int main(int argc, char const *argv[])
{
    printf("\nSO_HOLES: %d", SO_HOLES);
    printf("\nSO_TOP_CELLS: %d", SO_TOP_CELLS);
    printf("\nSO_SOURCES: %d", SO_SOURCES);    
    printf("\nSO_CAP_MIN: %d", SO_CAP_MIN);
    printf("\nSO_CAP_MAX: %d", SO_CAP_MAX);
    printf("\nSO_TAXI: %d", SO_TAXI);
    printf("\nSO_TIMENSEC_MIN: %lu", SO_TIMENSEC_MIN);
    printf("\nSO_TIMENSEC_MAX: %lu", SO_TIMENSEC_MAX);
    printf("\nSO_TIMEOUT: %d", SO_TIMEOUT);
    printf("\nSO_DURATION: %d\n", SO_DURATION);

    


    return 0;
}


