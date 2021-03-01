#include "header.h"

int main (int argc, char** argv)
{
    int sig = 0;
    sigset_t set;                       // Набор сигналов
    sigemptyset(&set);                  // Инициализирует набор сигналов, указанный в set, и "очищает" его от всех сигналов.
    sigaddset(&set, SIGUSR1);           // Добавляют сигнал к set
    sigprocmask(SIG_BLOCK, &set, NULL); // Изменяет список блокированных в данный момент сигналов


    key_t key = ftok( ".", 0); 
    int shmid = shmget(key, sizeof(shared_data), 0);
    if (shmid < 0) 
    {
        printf ("SHMID read child %d err", getpid());
        //exit (-1); 
    }
    shared_data* data = (shared_data*)shmat(shmid, NULL, 0);


    while (true)
    {
        sigwait(&set, &sig);        // Ждем сигнал указанный в set
        for (int i = 1; i < argc - 1; i++)
        {        
            for (int k = 0; argv[i][k] != '\0'; k++)
            {
                printf("%c\n", argv[i][k]);
                //printf("%d\n", data->output_manager);
                //sleep(1);
            }
        }

        //printf("A\n");
        
        if (kill(data->output_manager, SIGUSR2)) printf("Err in kill in child");   //посылка сигнала процессу 
        //kill(par, SIGUSR2); 
        //shmdt(data);                           // Отсоединяет общую память
    }
    return 0;
}