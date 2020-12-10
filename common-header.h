
#ifndef _COMMON-HEADER_H /* dubbio su cosa mettere */
#define _COMMON-HEADER_H

/* Definire struct messaggi / macro per le dimensioni ecc */

/* Riduce di 1 il valore del semaforo (acquisisce risorsa) */
int sem_P(int s_id, unsigned short sem_num);

/* Aumenta di 1 il valore del semaforo (rilascia risorsa) */
int sem_V(int s_id, unsigned short sem_num);

/* Esegue una semop */
int sem_cmd(int s_id, unsigned short sem_num, short sem_op, short sem_flg);

/* aggiungere mask ecc */

#endif /* _COMMON-HEADER_H */