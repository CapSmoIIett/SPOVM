#include <stdio.h>
#include <stdlib.h>

#include <ncurses.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h> 


#define MAXAMOUNT 7

char* Strings[] = {"a", "b", "c", "d", "e", "f", "g"}; 


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
   
    key_t key = ftok(".", 'a');                 // key_t ftok(const char *pathname, int proj_id);  
                                                    // Функция ftok использует файл с именем pathname 
                                                    // (которое должно указывать на существующий файл 
                                                    // к которому есть доступ) и младшие 8 бит proj_id 
                                                    // (который должен быть отличен от нуля) для создания 
                                                    // ключа с типом key_t, используемого в System V IPC 
                                                    // для работы с msgget(2), semget(2), и shmget(2).
    int shmid = shmget(key, MAXAMOUNT * sizeof(int), IPC_CREAT);    // Получаем адресс созданного сегмента общей памяти
                                                                            // shmid -  идентификатор сегмента
    if (shmid < 0) 
    {
        printf ("SMID1 err");
        exit (-1); 
    }   
                                                                            // IPC_PRIVATE является не полем, а типом key_t. 
                                                                            // Если это специфическое значение используется для key, 
                                                                            // то системный вызов игнорирует все, кроме 9-и младших 
                                                                            // битов shmflg, и создает новый сегмент разделяемой памяти
    void* shared_streams = shmat(shmid, NULL, 0);


    key_t mem_key = ftok(".", 'a');
    shmid = shmget(mem_key, sizeof(int), 0);
    if (shmid < 0) 
    {
        printf ("SMID2 err");
        exit (-1); 
    }  
    int* shared_counter = (int*)shmat(shmid, NULL, 0);
    
    char command = 0;
    pid_t streams[MAXAMOUNT] = {0};     // Массив с номерами потоков
    


    pid_t print_pid = fork();           // После fork потомок наследует присоединенный сегмент разделяемой памяти.
    if (print_pid == 0)
    {
        while(true)
        {
            //printf("XX\n");
            //counter = 
            //*shared_counter; 
            //printf("aa%d\n", *shared_counter);      
            
            /*if(counter > 0 && counter < MAXAMOUNT)
                for (int i = 0; i < counter; i++)
                {
                    printf("aa%d\n", counter);
                    //kill(((pid_t*)shared_streams)[i], SIGUSR1);   // посылка сигнала процессу
                    //sigwait(&set, &reciever);    // приостанавливает работу потока и ждёт сигнала
                }*/
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
                    exit(-1);
                } 
                else streams[counter++] = p;            // Увеличиваем счетчик
                shared_counter = &counter;       // записываем данные в общую память
                //shared_streams = (void*)streams;
   
                break;
            }            
            case '-': 
            {
                if (counter == 0) break;                // Проверка на наличие потоков
                kill(streams[--counter], SIGKILL);      // Убиваем дочерний процесс
                shared_counter = &counter;       // записываем новые данные в общую память
                //shared_streams = (void*)streams;        
                break;
            }   
            case 't' : printf("%d", *shared_counter); break;
            case 'q': case 'Q': 
            {
                while(counter--)
                {
                    kill(streams[counter], SIGKILL);
                }
                kill(print_pid, SIGKILL);
                exit (0);
            }           
        }
    }
    shmdt(shared_streams);       // Отсоединяет общую память. ???
    shmdt(shared_counter);
    return -2;
}