#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/shm.h>

char* Strings[] = {"a", "b", "c", "d", "e", "f", "g"}; 
int MaxAmount = 7;

//int signacion (SIGUSR1);

int main ()
{
    int reciever = 0;
    int counter = 0;

    sigset_t set;                       // набор сигналов
    sigemptyset(&set);                  // sigemptyset инициализирует набор сигналов, указанный в set, и "очищает" его от всех сигналов.
    sigaddset(&set, SIGUSR2);           // добовляем сигнал SIGUSR2; добавляem сигналы signum к set 
                                        // В POSIX-системах, SIGUSR1 и SIGUSR2 — пользовательские сигналы, 
                                        // которые могут быть использованы для межпроцессной синхронизации и управления
    sigprocmask(SIG_BLOCK, &set, NULL); // используется для того, чтобы изменить список блокированных в данный момент сигналов; получаем маcку сигнала текущего процесса
                                        // SIG_BLOCK- Набор блокируемых сигналов - объединение текущего набора и аргумента set.
   
    key_t key;
    int shmid = shmget(IPC_PRIVATE, MaxAmount * sizeof(int), IPC_CREAT);    // Получаем адресс созданного сегмента общей памяти
    // shmid -  идентификатор сегмента
    if (shmid == -1) exit (-1);    
    // IPC_PRIVATE является не полем, а типом key_t. 
    // Если это специфическое значение используется для key, 
    // то системный вызов игнорирует все, кроме 9-и младших 
    // битов shmflg, и создает новый сегмент разделяемой памяти.


    void* shared_memoryshmat(shmid, NULL, );
    
    
    char command = 0;
    pid_t streams[20] = {0};
    


    pid_t print_pid = fork();
    if (print_pid == 0)
    {
        while(true)
        {
            for (int i = 0; i < counter; i++)
            {
                kill(streams[i], SIGUSR1);   // посылка сигнала процессу
                sigwait(&set, &reciever);    // приостанавливает работу потока и ждёт сигнала
            }
        }
    }
    printf ("Entrer commands: ");
    while (true)
    {
        rewind(stdin);
        fflush(stdin);

        command = getchar();
        
        switch (command)
        {
            case '+': 
            {
                if (counter == MaxAmount) break;
                pid_t p = fork(); 
                if (p == 0) {execl("print", " ",  Strings[counter]); return 0;}
                else if (p == -1) exit(-1);
                else streams[counter++] = p;
                command = 'q';
                break;
            }            
            case '-': 
            {
                if (counter == 0) break;
                kill(streams[--counter], SIGKILL);          
                break;
            }   
            case 'q': case 'Q': 
            {
                while(counter--)
                {
                    kill(streams[counter], SIGKILL);
                }
                exit (0);
            }           
        }
    }
    shmdt(

    return 0;
}