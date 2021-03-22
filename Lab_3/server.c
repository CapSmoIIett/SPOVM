#include "header.h"

/*
    0-ой семафор отображает занята ли общая память другим процессом
    command - в общей памяти показывает требуемое действие 
*/

/*
    Сервер отправляет сообщение
*/

int main ()
{
    int semid;                    // идентификатор семафора 
    int shmid;                    // идентификатор разделяемой памяти 
    message_t *shared_memory;             // адрес сообщения в разделяемой памяти 
        char str[MAXLENGTH];

    semid = semget (SEM_ID, 1, PERMS | IPC_CREAT); // создание массива семафоров из одного элемента 
    if (semid < 0)
    {
        printf("server semaphore err");
        exit (-1);
    }
 
    shmid = shmget(SHM_ID, sizeof (message_t), PERMS | IPC_CREAT);    // Создаем сегмент общей памяти
    if (shmid < 0)
    {
        printf("server 1 shared memory err");
        exit(-1);
    }

    shared_memory = (message_t *) shmat (shmid, 0, 0);                      // Получаем адресс сегмента
    if (shared_memory == NULL)
    {
        printf("server 2 shared memory error");
        exit(-1);
    }

    semctl(semid, 0, SETVAL, 0);                                   // Устанавливаем семафор
    shared_memory->command = MSGEMPTY;


    while (1)
    {
        scanf ("%s", str);
        while (semctl (semid, 0, GETVAL, 0) || shared_memory->command != MSGEMPTY);      // Если 0-ой семафор занят или прошлое сообщение небыло обработанно - ждем                                                
        
        semctl (semid, 0, SETVAL, 1);               // Устанавливаем 0-й семафор, чтобы нам никто не мешал

        if (strcmp(str, ":q") == 0 )                // Выход в стиле Vim
        {
            shared_memory->command = MSGFINISH;     // Сообщаем о завершении работы
            semctl(semid, 0, SETVAL, 0);            // Снимаем блокировку (обнуляем 0-й семафор)
            break;
        }
 
                 
        strncpy (shared_memory->string, str, MAXLENGTH);    // Записываем строку в общую память
        shared_memory->command = MSGSTRING;                 // И даем команду на вывод

        semctl (semid, 0, SETVAL, 0);     // Снимаем блокировку 
    }

    while (semctl (semid, 0, GETVAL, 0) || shared_memory->command != MSGEMPTY);            // Ждем пока клиент заметит окончание программы и отреагирует на него
                                                                                           // нам нельзя сразу завершаться, так как здесь мы удаляем общую память и семафоры  
    semctl(semid, 0, IPC_RMID, (struct semid_ds *) 0);     // удаление массива семафоров 
    shmdt (shared_memory);
    shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);        // удаление сегмента разделяемой памяти 

    return 0;
}
