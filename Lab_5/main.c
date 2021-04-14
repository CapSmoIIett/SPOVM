#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <aio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>\


#define LIBADRESS  "/home/matthew/Projects/Laboratory/Курс 2/СПОВМ/Lab_5/lmylib.so"
#define NAMEFILE  "outfile.txt"
#define SEM_ID	2001      // ключ массива семафоров
#define SHM_ID	2002      // ключ разделяемой памяти 
#define PERMS	0666      // права доступа 
#define MAXLENGTH 250

typedef struct
{
    char string [MAXLENGTH];
} message_t;


pthread_mutex_t stdoutMutex;
int flagWork;                       // true -потоки работают, false - завершаются 


void  _print(char*);
void  closeThread (pthread_t);
void* threadReader(void*);
void* threadWriter(void*);
pthread_t addThread(void* (*)(void*));

int checkTxt (char*);


int main (){
    flagWork = 1;

    int semid = 0;                                                  // идентификатор семафора 
    int shmid = 0;                                                  // идентификатор разделяемой памяти 
    int ret = 0;

    if(pthread_mutex_init(&stdoutMutex, NULL) != 0) {               // Создаем mutex
        _print ("Error!\n");
        return 0;
    }

    semid = semget (SEM_ID, 3, PERMS | IPC_CREAT);                  // Создание массива семафоров из одного элемента 
    if (semid < 0) {
        printf("server semaphore err");
        exit (-1);
    }

    shmid = shmget(SHM_ID, sizeof (message_t), PERMS | IPC_CREAT);  // Создаем сегмент общей памяти
    if (shmid < 0) {
        printf("server 1 shared memory err");
        exit(-1);
    }

    semctl(semid, 0, SETVAL, 0);                                    // Устанавливаем значения семафоров на 0
    semctl(semid, 1, SETVAL, 0);
    semctl(semid, 2, SETVAL, 0);
    
    pthread_t thReader = addThread (threadReader);                  // Создаем поток читателя
    pthread_t thWrite  = addThread (threadWriter);                  // Создаем поток писателя
    
    //usleep(10000);

    

    usleep(100000);
    //ret = pthread_join(thReader, NULL);
    ret = pthread_join(thWrite, NULL);                             // Ждем окончания потока записи. Он закончится позже потока чтения 

    closeThread (thReader);                                         // Закрываем потоки
    closeThread (thWrite);

    semctl(semid, 0, IPC_RMID, (struct semid_ds *) 0);              // удаление массива семафоров 
    shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);                 // удаление сегмента разделяемой памяти 

    pthread_mutex_destroy(&stdoutMutex);                            // Уничтожаем mutex

    return 0;
}

void* threadReader(void* a){
    int semid;                                                      // идентификатор семафора 
    int shmid;                                                      // идентификатор разделяемой памяти 
    void* handle;
    char* (*fun) (char*);
    char *error;
    message_t *shared_memory;                                       // Адрес сообщения в разделяемой амяти 
    DIR * dir;

    struct dirent * de;

    semid = semget (SEM_ID, 3, 0);                                  // Получение доступа к массиву семафоров 
    if (semid < 0)
    {
        _print("Reader semaphore err");
        exit(-1);
    }
 
    shmid = shmget (SHM_ID, sizeof (message_t), 0);                 // Получение доступа к сегменту разделяемой памяти 
    if (shmid < 0)
    {
        _print("Reader shared memory err");
        exit(-1);
    }

    shared_memory = (message_t *) shmat (shmid, 0, 0);              // получение адреса сегмента 
    if (shared_memory == NULL){
        _print("Reader shared memory error");
        semctl(semid, 2, SETVAL, 1);                                // Сразу завершаем писателя
        exit (-1);
    }

    if ((dir = opendir(".")) == NULL) {                             // Просмотр файлов в директории
        _print("Forbidden, no permission!\n");
        semctl(semid, 2, SETVAL, 1);                                // Сразу завершаем писателя
        exit (-1);
    }

    handle = dlopen (LIBADRESS, RTLD_LAZY);                         // Загружаеm динамическую библиотеку

    if (!handle) {
        _print (dlerror());
        semctl(semid, 2, SETVAL, 1);                                // Сразу завершаем писателя
        exit (-1);
    }

    fun = dlsym (handle, "fileRead");                               // Ищет символы в нашей библиотеке. В данном случае функцию чтения 
    if ((error = dlerror ()) != NULL)  {
        _print(error);
        semctl(semid, 2, SETVAL, 1);                                // Сразу завершаем писателя
        exit (-1);
    }

    while ( de = readdir(dir) ) {                                   // Цикл по всем элемнтам в деректории
        //printf("%d\n", checkTxt(de->d_name));
        //_print(de->d_name);
        if (checkTxt(de->d_name) == 0 &&                            // Если это текстовый файл и
            strcmp(de->d_name, NAMEFILE) != 0) {                    // он не наш выводной файл

            while (semctl(semid, 0, GETVAL, 0) ||                   // Ждем пока память занята или прошлое сообщение не обработанно
                   semctl(semid, 1, GETVAL, 0) );

            semctl(semid, 0, SETVAL, 1);                            // Занимаем общую память

            char namefile[20];
            strncpy(namefile, de->d_name, 20);
            char* message = (*fun) (namefile);                    // Вызов библиотечной функции
            strncpy (shared_memory->string, message, MAXLENGTH);    // Записываем строку в общую память
            //_print (message);
                                                                    // Вывод полученного результата
            semctl(semid, 1, SETVAL, 1);                            // Устанавливаем, что нужно обработать сообщение
            semctl(semid, 0, SETVAL, 0);                            // Освободаем общую память
            free (message);                                         // Освобождаем полученное сообщение
        }
    }
    semctl(semid, 2, SETVAL, 1);                                    // Сообщаем писателю что пора заканчиваться

    if(dlclose (handle) != 0){                                      // dlclose уменьшает на единицу счетчик ссылок на указатель
        _print (dlerror());                                         // динамической библиотеки handle. Если нет других загруженных 
    }                                                               // библиотек, использующих ее символы и если счетчик ссылок 
                                                                    // принимает нулевое значение, то динамическая библиотека выгружается
    
    shmdt (shared_memory);                                          // Отсоединить сегмент разделяемой памяти 

    closedir(dir);                                                  // Закрываем директорию
    
    return (void*) NULL;
}

void* threadWriter (void* a){
    int semid;                                                      // идентификатор семафора 
    int shmid;                                                      // идентификатор разделяемой памяти 
    void* handle;
    int (*fun) (char*, char*);
    char *error;
    message_t *shared_memory;
    DIR * dir;

    struct dirent * de;

     semid = semget (SEM_ID, 3, 0);                                  // Получение доступа к массиву семафоров 
    if (semid < 0) {
        _print("Writer semaphore err");
        exit(1);
    }
 
    shmid = shmget (SHM_ID, sizeof (message_t), 0);                 // Получение доступа к сегменту разделяемой памяти 
    if (shmid < 0) { 
        _print("Writer shared memory err");
        exit(1);
    }

    shared_memory = (message_t *) shmat (shmid, 0, 0);              // получение адреса сегмента 
    if (shared_memory == NULL) {
        _print("Writer shared memory error");
        exit(1);
    }

     if ((dir = opendir(".")) == NULL) {                             // Просмотр файлов в директории
        _print("Forbidden, no permission!\n");
        exit (-1);
    }

    handle = dlopen (LIBADRESS, RTLD_LAZY);                         // загружаеv динамическую библиотеку

    if (!handle) {
        _print (dlerror());
        exit (1);
    }

    fun = dlsym (handle, "fileWrite");
    if ((error = dlerror ()) != NULL)  {
        _print (error);
        exit (1);
    }

    while ( de = readdir(dir) ){                                    // Если уже существует файл с тем же имене что и output
        if (strcmp(de->d_name, NAMEFILE))
            remove(NAMEFILE);                                       // Удаляем файл
    }
    
    while (!semctl(semid, 2, GETVAL, 0)){
        if(!semctl(semid, 0, GETVAL, 0) &&                          // Если память свободна и в памяти есть сообщение
            semctl(semid, 1, GETVAL, 0) ){
            semctl(semid, 0, SETVAL, 1);                            // Занимаем общкю память
            
            //_print (shared_memory->string);

            (*fun) (NAMEFILE, shared_memory->string);               // Записываем сообщение
            (*fun) (NAMEFILE, "\n");
            
            semctl(semid, 1, SETVAL, 0);                            // Устанавливаем, что сообщение обработанно
            semctl(semid, 0, SETVAL, 0);                            // Освободаем общую память
            
        }
    }
    

    if(dlclose(handle) != 0){    
        _print (dlerror());    
    }    

    shmdt (shared_memory);                                          // Отсоединить сегмент разделяемой памяти 
                              
    
    return (void*) NULL;
}


pthread_t addThread (void* (*fun)(void*)){
    pthread_t thread;                                                       // Индефикатор потока (указатель на поток)
    pthread_attr_t attr;                                                    // Парамметры потока
    pthread_attr_init (&attr);                                               // получаем дефолтные значения атрибутов 
    pthread_create (&thread, &attr, fun, NULL);        // Создаем поток
    return thread;
}
void closeThread (pthread_t thread){
    pthread_cancel (thread);    // Завершаем поток
}

void _print (char* msg){
    pthread_mutex_lock (&stdoutMutex);           // Мьютим вывод, чтобы не пересикались выводы
    printf ("%s\n", msg);
    pthread_mutex_unlock (&stdoutMutex);
}

int checkTxt (char* str){
    int len = 0;
    while (str[len++] != '\0');
    if (str[len- 4] == 't' &&
        str[len- 3] == 'x' &&
        str[len- 2] == 't')
        return 0;
    return -1;
}