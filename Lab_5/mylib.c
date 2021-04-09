#include <stdio.h>
#include <stdlib.h>
#include <aio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>

struct aiocb createIoRequest(int fd,  
                            off_t offset, 
                            volatile void * content, 
                            size_t length){
    // создает и инициализирует aiocb структуру
    // Если мы не инициализируем нулем, то у нас может быть неопределенный результат
               
    struct aiocb status = {0};
    {
        status.aio_fildes = fd;         // Дескриптор файла
        status.aio_offset = offset;     // Смещение файла
        status.aio_buf = content;       // Расположение буфера
        status.aio_nbytes = length;     // Длина передачи        
        status.aio_reqprio;             // Приоритет запроса 
        status.aio_sigevent;            // Способ уведомления 
        status.aio_lio_opcode;          // Операция, которую нужно выполнить
    }
    /*  union sigval {                  // Данные передаются с уведомлением 
            int sival_int;              // Целочисленное значение 
            void * sival_ptr;           // Значение указателя 
        };

        struct sigevent {
            int sigev_notify;           // Способ уведомления 
            int sigev_signo;            // Cигнал уведомления
            union sigval sigev_value;   // Данные передаются с уведомление 

            void (* sigev_notify_function) (объединение sigval); // Функция, используемая для потока
                                                                 // уведомление (SIGEV_THREAD) 
            void * sigev_notify_attributes; // Атрибуты для цепочки уведомлений (SIGEV_THREAD)
            pid_t sigev_notify_thread_id;   // ID потока для сигнала (SIGEV_THREAD_ID)
        };
    */
    return status;
}

char* fileRead(char* nameFile)
{
    FILE* file = fopen (nameFile, "r");                         // Открываем файл для чтения 
    int fd = fileno(file);                                      // Файловый дескриптор
    int status;                                                 // status обычно равен = 0
    char* buf;

    buf = (char*)calloc(200, sizeof(char));                     // alocate memory

    printf("Read\n");

    struct aiocb op  = createIoRequest(fd, 0, buf, 200);        // Задаем статус

    status = aio_read(&op);                                     // ставит в очередь запрос ввода-вывода, описанный буфером, на который указывает aiocb
    if(status) printf("aio_read_error: %d\n", status);

    const struct aiocb * aiolist[1];
    aiolist[0] = &op;

    status = aio_suspend(aiolist, 1, NULL);                     // Приостанавливает поток, пока не будут завершены операции в aiolist
    if (status) printf("aio_suspend_err: %d\n", status);        

    status = aio_error(&op);                                    // Возввращает статус ошибки
    if (status) printf("errno: %d\n", status);                  // 0 - все хорошо

    fclose(file);                                               // Закрыываем файл
    return buf;
}

int fileWrite(char* nameFile, char* message)
{
    FILE* file = fopen(nameFile,"a");                           // Открываем файл на изменение
    int fd = fileno(file);                                      // Файловый дескриптор
    
    int status = 0;
    int length = 0;

    while (message[length++] != '\0');                          // Считаем длинну сообщения

    printf("Write\n");

    struct aiocb op  = createIoRequest(fd, 0, message, length - 1); // Задаем статус

    status = aio_write(&op);                                    // ставит в очередь запрос ввода-вывода, описанный буфером, на который указывает aiocb
    if (status) printf("aio_write: %d\n", status);

    const struct aiocb * aiolist[1];
    aiolist[0] = &op;

    status = aio_suspend(aiolist,1,NULL);                       // Приостанавлиает поток, пока не будут завершены операции указаные в aiolist
    if (status) printf("aio_suspend: %d\n",status);

    status = aio_error(&op);                                    // Вощвращает статус ошибки 
    if (status) printf("errno 1: %d\n",status);                 // 0 - все хорошо        
    
    fclose(file);
    return 0;
}