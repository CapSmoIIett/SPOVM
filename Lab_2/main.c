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

char* Strings[] = {"a", "b", "c", "d", "e", "f", "g"}; 

typedef struct
{
  int counter;
  int streams[MAXAMOUNT];
} shared_data;


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
   

    
    char command = 0;
    pid_t streams[MAXAMOUNT] = {0};     // Массив с номерами потоков
    

    /*
    smget - создает память
    shmat - возвращает укзатель для работы с памятью
    shmdt - отсоединяет общую память
    shmctl - удаляет общую память
    */


    pid_t print_pid = fork();           // После fork потомок наследует присоединенный сегмент разделяемой памяти.
    if (print_pid == 0)
    {
         key_t key = ftok(".", 0); 
         int shmid = shmget(key, sizeof(shared_data), IPC_CREAT);
         if (shmid < 0) 
        {
            printf ("SHMID1 err");
            exit (-1); 
        }
        shared_data* data = (shared_data*)shmat(shmid, NULL, 0);
        while(true)
        {
            counter = data->counter;  
            if( counter > 0 &&  counter < MAXAMOUNT)
                for (int i = 0; i <  counter; i++)
                {
                    printf("aa%d\n",  counter);
                    //kill(((pid_t*)data)[i], SIGUSR1);   // посылка сигнала процессу
                    //sigwait(&set, &reciever);    // приостанавливает работу потока и ждёт сигнала
                    //printf("\n");
                }
        }
    }



    key_t key = ftok(".", 0);                 // key_t ftok(const char *pathname, int proj_id);  
                                                    // Функция ftok использует файл с именем pathname 
                                                    // (которое должно указывать на существующий файл 
                                                    // к которому есть доступ) и младшие 8 бит proj_id 
                                                    // (который должен быть отличен от нуля) для создания 
                                                    // ключа с типом key_t, используемого в System V IPC 
                                                    // для работы с msgget(2), semget(2), и shmget(2).
    int shmid = shmget(key, sizeof(shared_data), PERMS | IPC_CREAT);    // Получаем адресс созданного сегмента общей памяти
                                                                // shmid -  идентификатор сегмента
    if (shmid < 0) 
    {
        printf ("SHMID1 err");
        exit (-1); 
    }   
                                                    // IPC_PRIVATE является не полем, а типом key_t. 
                                                    // Если это специфическое значение используется для key, 
                                                    // то системный вызов игнорирует все, кроме 9-и младших 
                                                    // битов shmflg, и создает новый сегмент разделяемой памяти

    shared_data* data = (shared_data*)shmat(shmid, NULL, 0);   // shmat () присоединяет сегмент разделяемой памяти System V, 
                                                    // идентифицированный shmid, к адресному пространству вызывающего 
                                                    // процесса. Адрес присоединения указывается shmaddr с одним из следующих критериев           



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
                if (counter == MAXAMOUNT) break;                // Нельзя создать потоков больше чем MAXAMOUNT
                pid_t p = fork();           
                if (p == 0)                                     // Отправляем дочерний процесс выполнять программу print
                {
                    execl("print", " ",  Strings[counter]) ;    // После exec все присоединенные сегменты разделяемой памяти отсоединяются
                    return 0;
                }
                else if (p == -1)
                {
                    kill(print_pid, SIGKILL);
                    shmdt(data);      
                    exit(-1);
                } 
                else streams[counter++] = p;            // Увеличиваем счетчик
                data->counter = counter;                // записываем данные в общую память
                break;
            }            
            case '-': 
            {
                if (counter == 0) break;                // Проверка на наличие потоков
                kill(streams[--counter], SIGKILL);      // Убиваем дочерний процесс
                data->counter = & counter;              // записываем новые данные в общую память
        
                break;
            }   
            case 't' : printf("%d", data->counter); break;
            case 'q': case 'Q': 
            {
                while(counter--)
                {
                    kill(streams[counter], SIGKILL);
                }
                shmdt(data);  
                shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);  // Удаляет общую память   
                kill(print_pid, SIGKILL);
                exit (0);
            }           
        }
    }
    shmdt(data);                                    // Отсоединяет общую память
    shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);  // Удаляет общую память

    return -2;
}