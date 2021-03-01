#include <stdio.h>
#include <stdlib.h>

#include <ncurses.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h> 

#define SHM_ID	2002      /* ключ разделяемой памяти */
#define PERMS	0666      /* права доступа */


#define MAXAMOUNT 7


typedef struct
{
  int counter;              // Количество потоков вывода
  int output_manager;       // Номер потока регулирующий вывод
  int streams[MAXAMOUNT];   // Массив с номерами потоков
} shared_data;
