#include "header.h"
//#include <stdio.h>

void* threadReader(void*);
void* threadWriter(void*);

int main ()
{
    printf("Work\n");
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

void* threadReader(void* a)
{

    return (void*) NULL;
}

void* threadWriter(void* a)
{

    return (void*) NULL;
}

/*#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <termios.h>
#include <ncurses.h>
#include <pthread.h>


#define MAX_COUNT 100
#define MESSAGE "This process: "


void* printString(void* arg);
void CloseThread();
void WaitThreads();
void AddThread();

pthread_mutex_t PrintMutex;
pthread_t WorkingThreads[MAX_COUNT];
int NumThreads = 0;

/*
pthread_t           — идентификатор потока;
pthread_mutex_t     — мютекс;
pthread_mutexattr_t — объект атрибутов мютекса
pthread_cond_t      — условная переменная
pthread_condattr_t  — объект атрибута условной переменной;
pthread_key_t       — данные, специфичные для потока;
pthread_once_t      — контекст контроля динамической инициализации;
pthread_attr_t      — перечень атрибутов потока.
*/
/*


int main()
{
    char choice = 0;

    if(pthread_mutex_init(&PrintMutex, NULL) != 0)              // Создаем mutex
    {
        printf("Error!\n");
        return 0;
    }

    while(1)
    {
        choice = getchar();
        switch(choice)
        {

            case '+':
                    if (NumThreads < MAX_COUNT) { 
                        AddThread();
                    }
                    else {
                        pthread_mutex_lock(&PrintMutex);
                        printf("\r\nMax amount of process\n\n");
                        pthread_mutex_unlock(&PrintMutex);  
                    }
                    break;

            case '-': 
                    if (NumThreads > 0) { 
                        CloseThread(); 
                    }
                    else {
                        pthread_mutex_lock(&PrintMutex);
                        printf("\r\nNet Procesov\n\n");
                        pthread_mutex_unlock(&PrintMutex); 
                    } 
                    break;

            case 'q':
                    while(NumThreads > 0) CloseThread();
                    pthread_mutex_destroy(&PrintMutex);         // Уничтожаем mutex 
                    clear(); 
                    return 0;

            default: break;
        }
    }
}



void CloseThread()
{

    pthread_cancel(WorkingThreads[NumThreads - 1]);    // Завершаем поток
    //pthread_detach(WorkingThreads[NumThreads - 1]);
    //pthread_join(WorkingThreads[NumThreads - 1], NULL);     // ждем завершения b получаем значения
    NumThreads--;
}


void AddThread()
{
    pthread_t thread;                                                       // Индефикатор потока (указатель на поток)
    pthread_attr_t attr;                                                    // Парамметры потока
    pthread_attr_init(&attr);                                               // получаем дефолтные значения атрибутов 
    pthread_create(&thread, &attr, printString, (void*)&NumThreads);        // Создаем поток
    WorkingThreads[NumThreads] = thread;
    NumThreads++;
}



void* printString(void* arg)
{
    int threadNumber = *((int*)arg);   // Номер данного потока

    while(1)
    {
        pthread_mutex_lock(&PrintMutex);           // Мьютим вывод, чтобы не пересикались выводы
        printf("%s%d\n",MESSAGE, threadNumber);
        pthread_mutex_unlock(&PrintMutex);
        usleep(600000);     
    }
    return NULL;
}

*/
