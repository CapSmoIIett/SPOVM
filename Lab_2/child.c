#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h> 
#include <sys/wait.h>


int main (int argc, char** argv)
{
     int reciever = 0;
    sigset_t set;                       //набор сигналов
    sigemptyset(&set);                  //очищаем набор сигналов на который указывает 'set'
    sigaddset(&set, SIGUSR1);           //добовляем сигнал SIGUSR1
    sigprocmask(SIG_BLOCK, &set, NULL); //получаем маcку сигнала текущего процесса
    int pid = getpid();                 //получаем PID

    while (true)
    {
        for (int i = 1; i < argc - 1; i++)
        {
            sigwait(&set, &reciever); //ждём сигнала
            for (int k = 0; argv[i][k] != '\0'; k++)
            {
                putchar(argv[i][k]);
                putchar('\n');
                sleep(1);
               
            }
             kill(getppid(), SIGUSR2);   //посылка сигнала процессу
    
        }
    }
    return 0;
}