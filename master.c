#include "sem_lib.h"
#include "utility.h"

/* parametri definiti a tempo di compilazione */
#define SO_WIDTH 20
#define SO_HEIGHT 10

/* parametri definiti a tempo di esecuzione */
unsigned int SO_HOLES;
unsigned int SO_TOP_CELLS;
unsigned int SO_SOURCES;
unsigned int SO_CAP_MIN;
unsigned int SO_CAP_MAX;
unsigned int SO_TAXI;
unsigned int SO_TIMENSEC_MIN;
unsigned int SO_TIMENSEC_MAX;
unsigned int SO_TIMEOUT;
unsigned int SO_DURATION;

preset_dense()
{
    SO_WIDTH = 20;
    SO_HEIGHT = 10;
    SO_HOLES = 10;
    SO_TOP_CELLS = 40;
    SO_SOURCES = SO_WIDTH * SO_HEIGHT - SO_HOLES;
    SO_CAP_MIN = 1;
    SO_CAP_MAX = 1;
    SO_TAXI = SO_SOURCES / 2;
    SO_TIMENSEC_MIN = 100000000;
    SO_TIMENSEC_MAX = 300000000;
    SO_TIMEOUT = 1;
    SO_DURATION = 20;
}

preset_large()
{
    SO_WIDTH = 60;
    SO_HEIGHT = 20;
    SO_HOLES = 50;
    SO_TOP_CELLS = 40;
    SO_SOURCES = 10;
    SO_CAP_MIN = 3;
    SO_CAP_MAX = 5;
    SO_TAXI = 1000;
    SO_TIMENSEC_MIN = 10000000;
    SO_TIMENSEC_MAX = 100000000;
    SO_TIMEOUT = 3;
    SO_DURATION = 20;
}

int main(int argc, char const *argv[])
{
    /* scelta preset da riga di comando:
     * -large
     * -dense
     * 
     * senza prest, i parametri si settano direttamente nella creazione delle variabili globali
    */
    if (argc > 2)
    {
        if (argv[1] == "-large")
            preset_large();

        else if (argv[1] == "-dense")
            preset_dense();
    }
    

    return 0;
}
