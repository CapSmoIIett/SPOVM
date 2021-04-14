//#include "mylib.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#define LIBADRESS  "/home/matthew/Projects/Laboratory/Курс 2/СПОВМ/Lab_6/mylib.so"


void printArray(int*, int);

int main ()
{
    void* handle;
    char* error;
    void (* init) ();
    void (*_free) (void*);
    void*(*_malloc)(long);
    void*(*_calloc)(long, long);
    void*(*_realloc)(void*, long);

    handle = dlopen (LIBADRESS, RTLD_LAZY);     // Загружаеm динамическую библиотеку

    if (!handle) {
        printf ("%s", dlerror());
        exit (-1);
    }

    init = dlsym (handle, "malloc_init");       // Ищет символы в нашей библиотеке. В данном случае функцию чтения 
    if ((error = dlerror ()) != NULL)  {
        printf ("%s", error);
        exit (-1);
    }

    _free = dlsym (handle, "free");       // Ищет символы в нашей библиотеке. В данном случае функцию чтения 
    if ((error = dlerror ()) != NULL)  {
        printf ("%s", error);
        exit (-1);
    }

    _malloc = dlsym (handle, "malloc");       // Ищет символы в нашей библиотеке. В данном случае функцию чтения 
    if ((error = dlerror ()) != NULL)  {
        printf ("%s", error);
        exit (-1);
    }

    _calloc = dlsym (handle, "calloc");       // Ищет символы в нашей библиотеке. В данном случае функцию чтения 
    if ((error = dlerror ()) != NULL)  {
        printf ("%s", error);
        exit (-1);
    }
    _realloc = dlsym (handle, "realloc");       // Ищет символы в нашей библиотеке. В данном случае функцию чтения 
    if ((error = dlerror ()) != NULL)  {
        printf ("%s", error);
        exit (-1);
    }

    //init();                                         // 
    

    int len = 10;
    int* array = (int*)_malloc(sizeof(int) * len);
    
    printArray(array, len);
    for (int i = 0; i < len; i++){
        array[i] = i + 2;
    }
    printArray(array, len);

    _free(array);

    len -= 3;
    array = (int*)_realloc(array, sizeof(int) * len);         // Уменьшаемы выделенную память
    printArray(array, len);

    _free(array);

    len += 3;
    array = (int*)_realloc(array, sizeof(int) * len);         // Увеличиваем выделенную память
    printArray(array, len);

    _free(array);

    array = (int*)_calloc(sizeof(int), len);       // Пример показывающий что освобожденная память может использоваться повторно
    printArray(array, len);


 

    _free(array);
    return 0;
}

void printArray (int* ms, int len){
    for (int i = 0; i < len; i++){
        printf("%d ", ms[i]);
    }
    printf("\n");
}