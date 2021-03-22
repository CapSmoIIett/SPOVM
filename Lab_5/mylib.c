#include <stdio.h>
#include <stdlib.h>
//#include <aio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>



int fileRead();
int fileWrite();

int fileRead()
{
    printf("Read\n");
    return 100;
}
int fileWrite()
{
    printf("Write\n");
    return 0;
}