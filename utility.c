#include "utility.h"

void print_resource(int id_sem)
{
    int i, j = 1;

    printf("  ");
    for (i = 0; i < NUM_RISORSE; i++)
    {
        printf("%2d ", semctl(id_sem, i, GETVAL));
        if (j % SO_WIDTH == 0)
        {
            printf("\n  ");
        }
        j++;
    }
    printf("\n");
}

void print_matrix(struct shared_map *mat, int field)
{
    int i, j;

    if (field != 1 && field != 2 && field != 3 && field != 4 && field != 5)
    {
        fprintf(stderr, "Stampa: errore nel parametro field\n");
        return;
    }

    switch (field)
    {
    case 1:
        printf("Stampa: crossing_time\n\n");
        break;

    case 2:
        printf("Stampa: crossing_cont\n\n");
        break;

    case 3:
        printf("Stampa: is_hole\n\n");
        break;

    case 4:
        printf("Stampa: nmax_taxi\n\n");
        break;
    case 5:
        printf("Stampa: is_request\n\n");
        break;
    }

    for (i = 0; i < SO_HEIGHT; i++)
    {
        printf("  ");
        for (j = 0; j < SO_WIDTH; j++)
        {
            switch (field)
            {
            case 1:
                printf("%2ld ", mat->matrix[i][j].crossing_time);
                break;

            case 2:
                printf("%2d ", mat->matrix[i][j].crossing_cont);
                break;

            case 3:
                printf("%2d ", mat->matrix[i][j].is_hole);
                break;

            case 4:
                printf("%2d ", mat->matrix[i][j].nmax_taxi);
                break;

            case 5:
                printf("%2d ", mat->matrix[i][j].request_pid);
                break;
            }
        }
        printf("\n");
    }
    printf("\n");
}

void print_status(struct shared_map *mat, int id_sem_cap)
{
    int i, j, sem_value = 0;

    for (i = 0; i < SO_HEIGHT; i++)
    {
        for (j = 0; j < SO_WIDTH; j++)
        {
            if (mat->matrix[i][j].is_hole)
            { 
                printf("  H ");
            }
            else
            {
                do
                {
                    if (errno == EINTR)
                        errno = 0;
                    TEST_ERROR
                    sem_value = semctl(id_sem_cap, INDEX(i, j), GETVAL);
                } while (errno == EINTR);
                TEST_ERROR
                printf("%2d ", mat->matrix[i][j].nmax_taxi - sem_value);
            }
        }
        printf("\n");
    }
    printf("\n");
}