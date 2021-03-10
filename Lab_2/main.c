#include "header.h"

char* Strings[] = {"a", "b", "c", "d", "e", "f", "g"}; 
#define SEM_ID	2001                    // ключ массива семафоров

int main ()
{
    int sig = 0;
    int semid = 0;
    //int counter = 0;

    sigset_t set;                       // набор сигналов(маска)
                                        // Маска сигналов - это набор сигналов, доставка которых
                                        // в настоящее время заблокирован для вызывающего абонента.
    sigemptyset(&set);                  // sigemptyset инициализирует набор сигналов, указанный в set, и "очищает" его от всех сигналов.
    sigaddset(&set, SIGUSR2);           // добовляем сигнал SIGUSR2; добавляem сигналы signum к set 
                                        // В POSIX-системах, SIGUSR1 и SIGUSR2 — пользовательские сигналы, 
                                        // которые могут быть использованы для межпроцессной синхронизации и управления
    sigprocmask(SIG_BLOCK, &set, NULL); // используется для того, чтобы изменить список блокированных в данный момент сигналов; 
                                        // Получаем маcку(sigset_t) сигнала текущего процесса
                                        // SIG_BLOCK- Набор блокируемых сигналов - объединение текущего набора и аргумента set.
   
    semid = semget (SEM_ID, 1, 0666 | IPC_CREAT); // создание массива семафоров из одного элемента 
    if (semid < 0)
    {
        printf("server semaphore err");
        exit (-1);
    }
 

    
    char command = 0;  
    

    /*
    smget - создает память
    shmat - возвращает укзатель для работы с памятью
    shmdt - отсоединяет общую память
    shmctl - удаляет общую память

    ftok - преобразует имя файла с идентификатором в ключ

    */


    pid_t print_pid = fork();           // После fork потомок наследует присоединенный сегмент разделяемой памяти.
    if (print_pid < 0) exit (-100);
    if (print_pid == 0)
    {
        key_t key = ftok(".", 0); 
        int shmid = shmget(key, sizeof(shared_data), 0);
        if (shmid < 0) 
        {
            printf ("SHMID read err");
            exit (-1); 
        }
        shared_data* data = (shared_data*)shmat(shmid, NULL, 0);
        data->output_manager = getpid();
        
        while(true)
        {
            sleep(1);
            while(semctl (semid, 0, GETVAL, 0));
            semctl (semid, 0, SETVAL, 1);
            int counter = data->counter;  
            if( counter > 0 &&  counter < MAXAMOUNT)
            {
                printf("\n");
                for (int i = 0; i < counter; i++)
                {
                    //sleep(1);
                    usleep(50000);
                    if (kill(data->streams[i], SIGUSR1))printf("Err in kill");   // посылка сигнала процессу
                    sigwait(&set, &sig);    // приостанавливает работу потока и ждёт сигнала
                                        
                }
            }
            semctl (semid, 0, SETVAL, 0);
        }
    }



    key_t key = ftok(".", 0);                 // key_t ftok(const char *pathname, int proj_id);  
                                                    // Функция ftok использует файл с именем pathname 
                                                    // (которое должно указывать на существующий файл 
                                                    // к которому есть доступ) и младшие 8 бит proj_id 
                                                    // (который должен быть отличен от нуля) для создания 
                                                    // ключа с типом key_t, используемого в System V IPC 
                                                    // для работы с msgget(2), semget(2), и shmget(2).
    int shmid = shmget( key, sizeof(shared_data), PERMS | IPC_CREAT);    // Получаем адресс созданного сегмента общей памяти
                                                                // shmid -  идентификатор сегмента
    if (shmid < 0) 
    {
        printf ("SHMID write err");
        exit (-1); 
    }   
                                                    // IPC_PRIVATE является не полем, а типом key_t. 
                                                    // Если это специфическое значение используется для key, 
                                                    // то системный вызов игнорирует все, кроме 9-и младших 
                                                    // битов shmflg, и создает новый сегмент разделяемой памяти

    shared_data* data = (shared_data*)shmat(shmid, NULL, 0);   // shmat () присоединяет сегмент разделяемой памяти System V, 
                                                    // идентифицированный shmid, к адресному пространству вызывающего 
                                                    // процесса. Адрес присоединения указывается shmaddr с одним из следующих критериев           

    data->counter = 0;



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
                
                int counter = data->counter;
                if (counter >= MAXAMOUNT) break;                // Нельзя создать потоков больше чем MAXAMOUNT

                while(semctl (semid, 0, GETVAL, 0));
                semctl (semid, 0, SETVAL, 1);

                pid_t p = fork();         
                if (p < 0)
                {
                    printf ("Something goes wrong");
                    semctl (semid, 0, SETVAL, 0);  
                }
                if (p == 0)                                     // Отправляем дочерний процесс выполнять программу print
                {
                    //printf("bb%d\n",  counter);
                    //execl("print", " ",  Strings[counter]) ;    // После exec все присоединенные сегменты разделяемой памяти отсоединяются
              
                    execl("print", " ", Strings[counter]) ;
                    return 0;
                }
                else if (p == -1)
                {
                    kill(print_pid, SIGKILL);
                    shmdt(data);      
                    exit(-1);
                } 
                else data->streams[counter++] = p;            // Увеличиваем счетчик
                data->counter = counter;                // записываем данные в общую память
                semctl (semid, 0, SETVAL, 0);
                break;
            }            
            case '-': 
            {
                while(semctl (semid, 0, GETVAL, 0));
                semctl (semid, 0, SETVAL, 1);
                if (data->counter == 0) break;                // Проверка на наличие потоков
                kill(data->streams[--data->counter], SIGKILL);      // Убиваем дочерний процесс
                semctl (semid, 0, SETVAL, 0 );
                break;
            }   
            case 't' : printf("%d", data->counter); break;
            case 'q': case 'Q': 
            {
                while(data->counter--)
                {
                    kill(data->streams[data->counter], SIGKILL);
                }
                shmdt(data);  
                shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);  // Удаляет общую память   
                semctl(semid, 0, IPC_RMID, (struct semid_ds *) 0);     // удаление массива семафоров 
                kill(print_pid, SIGKILL);
                 
                exit (0);
            }  
            default: break;        
        }
    }
    semctl(semid, 0, IPC_RMID, (struct semid_ds *) 0);     // удаление массива семафоров 
    shmdt(data);                                    // Отсоединяет общую память
    shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);  // Удаляет общую память

    return -2;
}
