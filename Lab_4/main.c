#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <termios.h>
#include <ncurses.h>
#include <pthread.h>


#define MAX_COUNT 10
#define MESSAGE "This process: "


void* printString(void* arg);
void CloseThread();
void WaitThreads();
void AddThread();

pthread_mutex_t PrintMutex;
pthread_t WorkingThreads[MAX_COUNT];
pthread_t ClosingThreads[MAX_COUNT];
int NumThreads = 0;
int QuitFlagsTreads[MAX_COUNT] = {0};

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



int main()
{
    if(pthread_mutex_init(&PrintMutex, NULL) != 0)
    {
        printf("Error!\n");
        endwin(); 
        return 0;
    }

    //initscr();
    //clear();
    //noecho();
    //refresh();

    while(1)
    {
        switch(getch())
        {
            case '+': if(NumThreads < MAX_COUNT) AddThread();
                      else printf("\r\nToo much processes! Tou can't add anymore!\n\n");  break;

            case '-': if(NumThreads > 0) CloseThread(); 
                      else printf("\r\nNo more processes!\n\n"); break;

            case 'q':
                while(NumThreads > 0) CloseThread();
                pthread_mutex_destroy(&PrintMutex);
                clear(); 
                //endwin(); 
                return 0;

            default: break;
        }
    }
}


void CloseThread()
{
    pthread_cancel(WorkingThreads[NumThreads - 1]);    // Завершаем поток
    pthread_join(WorkingThreads[NumThreads - 1], NULL);     // ждем завершения
    NumThreads--;
}


void AddThread()
{
    pthread_t thread;
    pthread_create(&thread, NULL, printString, (void*)&NumThreads);
    WorkingThreads[NumThreads] = thread;
    NumThreads++;
}



void* printString(void* arg)
{
    int threadNumber = *((int*)arg);   // Номер данного потока

    while(1)
    {
        if(QuitFlagsTreads[threadNumber]) break;
        pthread_mutex_lock(&PrintMutex);
        printf("%s%d\n",MESSAGE, threadNumber);
        usleep(45000);
        pthread_mutex_unlock(&PrintMutex);
    }
    return NULL;
}


