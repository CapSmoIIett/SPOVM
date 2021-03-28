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

    // E.g. through sigevent op.aio_sigevent there could be 
    //      a callback function being set, that the program
    //      tries to call - which will then fail.
    struct aiocb status = {0};
    {
        status.aio_fildes = fd;
        status.aio_offset = offset;
        status.aio_buf = content;
        status.aio_nbytes = length;            
    }
    return status;
}

char* fileRead()
{
    FILE* file = fopen("outfile.txt","r");
    int fd = fileno(file);                                      // Файловый дескриптор
    int status;                                                 // status basically equal 0
    char* buf;

    buf = (char*)calloc(20, sizeof(char));                     // alocate memory

    printf("Read\n");

    struct aiocb op  = createIoRequest(fd, 0, buf, 20);        

    status = aio_read(&op);
    if(status) printf("aio_read_error: %d\n", status);

    const struct aiocb * aiolist[1];
    aiolist[0] = &op;

    status = aio_suspend(aiolist, 1, NULL);
    if (status) printf("aio_suspend_err: %d\n", status);

    status = aio_error(&op);
    if (status) printf("errno: %d\n", status);        // report possible errors

    fclose(file);
    return buf;
}

int fileWrite(char* message, int size)
{
    FILE* file = fopen("outfile.txt","w");
    int fd = fileno(file);                                      // Файловый дескриптор
    
    int status;

    printf("Write\n");

    struct aiocb op  = createIoRequest(fd, 0, message, size);        

    status = aio_write(&op);
    if (status) printf("aio_write: %d\n", status);

    const struct aiocb * aiolist[1];
    aiolist[0] = &op;

    status = aio_suspend(aiolist,1,NULL);
    if (status) printf("aio_suspend: %d\n",status);

    status = aio_error(&op);
    if (status) printf("errno 1: %d\n",status);        // report possible errors
    
        
    
    fclose(file);
    return 0;
}