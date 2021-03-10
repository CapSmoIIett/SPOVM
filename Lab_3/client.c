#include <stdio.h>
#include <string.h>
#include "header.h"

/*
    Клиент принимает сообщение
*/

int main ()
{
    int semid;                    // идентификатор семафора 
    int shmid;                    // идентификатор разделяемой памяти 
    message_t *shared_memory;             // адрес сообщения в разделяемой амяти 

    semid = semget (SEM_ID, 1, 0); // получение доступа к массиву семафоров 
    if (semid < 0)
    {
        printf("client semaphore err");
        exit(-1);
    }
 
    shmid = shmget (SHM_ID, sizeof (message_t), 0);  // получение доступа к сегменту разделяемой памяти */
    if (shmid < 0)
    {
        printf("client 1 shared memory err");
        exit(-1);
    }

    shared_memory = (message_t *) shmat (shmid, 0, 0);   // получение адреса сегмента 
    if (shared_memory == NULL)
    {
        printf("client 2 shared memory error");
        exit(-1);
    }

    while (1)
    {
        if (shared_memory->command != MSGEMPTY)
        {
            if (semctl(semid, 0, GETVAL, 0))  continue;            // Если 0-ой семафор занят - ждем 
               

            semctl(semid, 0, SETVAL, 1);                           // Устанавливаем 0-й семафор, чтобы нам никто не мешал

            // Реагируем на сообщение в зависимости от флага (флаг отображает задание)
            if (shared_memory->command == MSGSTRING) printf ("%s\n", shared_memory->string);   
            if (shared_memory->command == MSGFINISH) 
            {
                shared_memory->command = MSGEMPTY;
                semctl(semid, 0, SETVAL, 0);          // Реагируем на окончание программы
                break;
            }
            shared_memory->command = MSGEMPTY;         // Обнуляем флаг
            semctl(semid, 0, SETVAL, 0);        // Снимаем семафор 
        }
    }


    shmdt (shared_memory);                /* отсоединить сегмент разделяемой памяти */
    exit (0);
}