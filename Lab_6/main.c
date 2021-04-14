//#include "mylib.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#define LIBADRESS  "/home/matthew/Projects/Laboratory/Курс 2/СПОВМ/Lab_6/mylib.so"



int main ()
{
    void* handle;
    char* error;
    void (* init) ();
    void (*_free) (void*);
    void*(*_malloc)(long);

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

    init();                                         // 
    //malloc_init();

    return 0;
}