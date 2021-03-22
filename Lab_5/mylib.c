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
    // Если мы не инициализируем нулем, то у нас бужет неопределенный результат

    // E.g. through sigevent op.aio_sigevent there could be 
    //      a callback function being set, that the program
    //      tries to call - which will then fail.
    struct aiocb ret = {0};
    {
        ret.aio_fildes = fd;
        ret.aio_offset = offset;
        ret.aio_buf = content;
        ret.aio_nbytes = length;            
    }
    return ret;
}

int fileRead();
int fileWrite();

int fileRead()
{
    printf("Read\n");
    return 100;
}
int fileWrite()
{
    FILE* file = fopen("outfile.txt","w");
    int fd = fileno(file);                                      // Файловый дескриптор
    {
        struct aiocb op  = createIoRequest(fd,0,"Shit\n", 5);        

        //aio_write(&op);
        int ret = aio_write(&op);
        printf("aio_write 1: %d\n",ret);

        {
            const struct aiocb * aiolist[1];
            aiolist[0] = &op;

            int ret = aio_suspend(aiolist,1,NULL);
            printf("aio_suspend: %d\n",ret);

            // report possible errors
            {
                ret = aio_error(&op);
                printf("errno 1: %d\n",ret);
            }
        }
    }
    fclose(file);

    printf("Write\n");
    return 0;
}