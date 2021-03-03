#include <stdio.h>
#include <string.h>
#include "header.h"


int main ()
{
    int semid;                    // идентификатор семафора 
    int shmid;                    // идентификатор разделяемой памяти 
    key_t key;
    message_t *msg_p;             // адрес сообщения в разделяемой амяти 
    char s[MAXLENGTH];

    semid = semget (SEM_ID, 1, 0); // получение доступа к массиву семафоров 
    if (semid < 0)
    {
        printf("client semaphore err");
        exit(-1);
    }

    shmid = shmget (SHM_ID, sizeof (message_t), 0);  // получение доступа к сегменту разделяемой памяти */
    if (shmid < 0)
    {
        printf("client shared memory err");
        exit(-1)
    }

    msg_p = (message_t *) shmat (shmid, 0, 0);   // получение адреса сегмента 
    if (msg_p == NULL)
    {
        printf("client shared memory error");
        exit(-1);
    }

    while (true)
    {
        scanf ("%s", s);
        while (semctl (semid, 0, GETVAL, 0) || msg_p->type != MSG_TYPE_EMPTY);
                /*
                *   если сообщение не обработано или сегмент блокирован - ждать
                *                                                             */
        
        semctl (semid, 0, SETVAL, 1);     /* блокировать */
        if (strlen (s) != 1)
        {
                /* записать сообщение "печать строки" */
            msg_p->type = MSG_TYPE_STRING;
            strncpy (msg_p->string, s, MAX_STRING);
        }
        else
        {
                /* записать сообщение "завершение работы" */
            msg_p->type = MSG_TYPE_FINISH;
            };
        semctl (semid, 0, SETVAL, 0);     /* отменить блокировку */
        if (strlen (s) == 1) break;
    }
    shmdt (msg_p);                /* отсоединить сегмент разделяемой памяти */
    exit (0);
}