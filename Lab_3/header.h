#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h> 
#include <stdio.h>


#define SEM_ID	2001      /* ключ массива семафоров */
#define SHM_ID	2002      /* ключ разделяемой памяти */
#define PERMS	0666      /* права доступа */

/* коды сообщений */

#define MSG_TYPE_EMPTY  0 /* пустое сообщение */
#define MSG_TYPE_STRING 1 /* тип сообщения о том, что передана непустая строка */
#define MSG_TYPE_FINISH 2 /* тип сообщения о том, что пора завершать обмен */
#define MAXLENGTH	10

/* структура сообщения, помещаемого в разделяемую память */
typedef struct
{
    int type;
    char string [MAXLENGTH];
} message_t;
