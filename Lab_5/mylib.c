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

char* fileRead(char* nameFile)
{
    FILE* file = fopen (nameFile, "r");
    int fd = fileno(file);                                      // Файловый дескриптор
    int status;                                                 // status basically equal 0
    char* buf;

    buf = (char*)calloc(200, sizeof(char));                     // alocate memory

    printf("Read\n");

    struct aiocb op  = createIoRequest(fd, 0, buf, 200);        

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

int fileWrite(char* nameFile, char* message)
{
    FILE* file = fopen(nameFile,"a");
    int fd = fileno(file);                                      // Файловый дескриптор
    
    int status = 0;
    int length = 0;

    while (message[length++] != '\0');

    printf("Write\n");



    struct aiocb op  = createIoRequest(fd, 0, message, length - 1);        

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