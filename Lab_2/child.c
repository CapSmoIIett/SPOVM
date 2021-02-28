#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h> 
#include <sys/wait.h>

// Эта программа компилируется в print

int main (int argc, char** argv)
{
    int sig = 0;
    sigset_t set;                       // Набор сигналов
    sigemptyset(&set);                  // Инициализирует набор сигналов, указанный в set, и "очищает" его от всех сигналов.
    sigaddset(&set, SIGUSR1);           // Добавляют сигнал к set
    //sigprocmask(SIG_BLOCK, &set, NULL); // Изменяет список блокированных в данный момент сигналов

    while (true)
    {
        for (int i = 1; i < argc - 1; i++)
        {
            //sigwait(&set, &sig);        // Ждем сигнал указанный в set
            for (int k = 0; argv[i][k] != '\0'; k++)
            {
                putchar(argv[i][k]);
                putchar('\n');
                sleep(1);
               
            }
            //kill(getppid(), SIGUSR2);   //посылка сигнала процессу
    
        }
    }
    return 0;
}