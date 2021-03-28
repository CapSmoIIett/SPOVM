#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <aio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>

#define LIBADRESS  "/home/matthew/Projects/Laboratory/Курс 2/СПОВМ/Lab_5/lmylib.so"

pthread_mutex_t stdoutMutex;

void* threadReader(void*);
void* threadWriter(void*);
pthread_t addThread(void* (*)(void*));
void closeThread (pthread_t);

void _print(char*);


int main (){
    if(pthread_mutex_init(&stdoutMutex, NULL) != 0) {             // Создаем mutex
        _print ("Error!\n");
        return 0;
    }

    pthread_t thWrite  = addThread (threadWriter);
    
    usleep(10000);

    pthread_t thReader = addThread (threadReader);
    

    usleep(10000);

    closeThread (thReader);
    closeThread (thWrite);
    //threadReader(NULL);
    pthread_mutex_destroy(&stdoutMutex);         // Уничтожаем mutex

    //threadWriter(NULL);           
    //fileWrite();
    /*pthread_t pthRead;
    pthread_t pthWrt;                                                       // Индефикатор потока (указатель на поток)
    
    pthread_attr_t attr1;                                                    // Парамметры потоков
    pthread_attr_t attr2;                                              
    
    pthread_attr_init(&attr1);                                               // получаем дефолтные значения атрибутов 
    pthread_attr_init(&attr2);                                            

    pthread_create(&pthRead, &attr1, threadReader, (void*)NULL);                      // Создаем поток читателя
    pthread_create(&pthWrt,  &attr2, threadWriter, (void*)NULL);                      // Создаем поток писателя


    pthread_cancel(pthRead);                        // Завершаем потоки
    pthread_cancel(pthWrt); */ 
    return 0;
}

void* threadReader(void* a){
    void* handle;
    char* (*fun) ();
    char *error;

    handle = dlopen (LIBADRESS, RTLD_LAZY);  // загружаеm динамическую библиотеку

    if (!handle) {
        _print (dlerror());
        exit (1);
    }


    fun = dlsym (handle, "fileRead");
    if ((error = dlerror ()) != NULL)  {
        _print(error);
        exit (1);
    }

    char* message = (*fun) ();

    _print (message);

    if(dlclose (handle) != 0){       // dlclose уменьшает на единицу счетчик ссылок на указатель
        _print (dlerror());    // динамической библиотеки handle. Если нет других загруженных 
    }                               // библиотек, использующих ее символы и если счетчик ссылок 
                                    // принимает нулевое значение, то динамическая библиотека выгружается
    
    free (message);
    return (void*) NULL;
}

void* threadWriter (void* a){
    void* handle;
    int (*fun) (char*, int );
    char *error;

    handle = dlopen (LIBADRESS, RTLD_LAZY);  // загружаеv динамическую библиотеку

    if (!handle) {
        _print (dlerror());
        exit (1);
    }

    fun = dlsym (handle, "fileWrite");
    if ((error = dlerror ()) != NULL)  {
        _print (error);
        exit (1);
    }
 
    (*fun) ("Am I evil?\n", 11);

    if(dlclose(handle) != 0){    
        _print (dlerror());    
    }                                  
    
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