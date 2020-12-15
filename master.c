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
    #ifdef DENSE
    /*Creo le variabili d'ambiente*/
    /*int setenv(const char *name, const char *value, int overwrite);*/
    setenv ("SO_HOLES", "10", 1); 
    setenv ("SO_TOP_CELLS", "40", 1);
    SO_HOLES = atoi(getenv("SO_HOLES")tempo cuzion
    H * SO_HEIGHT - SO_HOLES;
    setenv ("SO_SOURCES", "" + app, 1);   
    setenv ("SO_CAP_MIN", "1", 1); 
    setenv ("SO_CAP_MAX", "1", 1);                              
    SO_SOURCES = atoi(getenv("SO_SOURCES"));
    int app2 = SO_SOURCES/2;
    setenv ("SO_TAXI", "" + app2, 1);                       
    setenv ("SO_TIMENSEC_MIN", "100000000", 1); 
    setenv ("SO_TIMENSEC_MAX", "300000000", 1);  
    setenv ("SO_TIMEOUT", "1", 1); 
    setenv ("SO_DURATION", "20", 1); 
    /*Prendo i valori dalle variabili d'ambiente*/
    SO_HOLES = atoi(getenv("SO_HOLES"));
    SO_TOP_CELLS = atoi(getenv("SO_TOP_CELLS"));
    SO_SOURCES = atoi(getenv("SO_SOURCES"));
    SO_CAP_MIN = atoi(getenv("SO_CAP_MIN"));
    SO_CAP_MAX = atoi(getenv("SO_HOLES"));
    SO_TAXI = atoi(getenv("SO_TAXI"));
    SO_TOP_CELLS = atoi(getenv("SO_TOP_CELLS"));
    SO_TIMENSEC_MIN = atol(getenv("SO_TIMENSEC_MIN"));
    SO_TIMENSEC_MAX = atol(getenv("SO_TIMENSEC_MAX"));
    SO_TIMEOUT = atoi(getenv("SO_TIMEOUT"));
    SO_DURATION = atoi(getenv("SO_DURATION"));
    #else
    /*Acquisisco i valori da tastiera*/
    printf("SO_HOLES: ");
    scanf("%u", &SO_HOLES);
    printf("SO_TOP_CELLS: ");
    scanf("%u", &SO_TOP_CELLS);
    printf("SO_SOURCES: ");
    scanf("%u", &SO_SOURCES);
    printf("SO_CAP_MIN: ");
    scanf("%u", &SO_CAP_MIN);
    printf("SO_CAP_MAX: ");
    scanf("%u", &SO_CAP_MAX);
    printf("SO_TAXI: ");
    scanf("%u", &SO_TAXI);
    printf("SO_TIMENSEC_MIN: ");
    scanf("%lu", &SO_TIMENSEC_MIN);
    printf("SO_TIMENSEC_MAX: ");
    scanf("%lu", &SO_TIMENSEC_MAX);
    printf("SO_TIMEOUT: ");
    scanf("%u", &SO_TIMEOUT);
    printf("SO_DURATION: ");
    scanf("%u", &SO_DURATION);
    #endif
    
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


