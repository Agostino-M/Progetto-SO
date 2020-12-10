#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <signal.h>
#include "common-header.h"

int sem_signal(int s_id, unsigned short sem_num)
{
    struct sembuf sops;
    sops.sem_flg = 0;
    sops.sem_op = 1;
    sops.sem_num = sem_num;
    return semop(s_id, &sops, 1);
}

int sem_wait(int s_id, unsigned short sem_num)
{
    struct sembuf sops;
    sops.sem_flg = 0;
    sops.sem_op = -1;
    sops.sem_num = sem_num;
    return semop(s_id, &sops, 1);
}

int sem_cmd(int s_id, unsigned short sem_num, short sem_op, short sem_flg)
{
    struct sembuf sops;
    sops.sem_flg = sem_flg;
    sops.sem_op = sem_op;
    sops.sem_num = sem_num;
    return semop(s_id, &sops, 1);
}