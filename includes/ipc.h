#ifndef _IPC_H_
#define _IPC_H_

#include <semaphore.h>
#include <sys/mman.h>

#define SHMEM_SIZE 16384
#define SHMEM_NAME "md5_shmem"
#define SEM_READ_NAME "md5_semaphore_read"
#define SEM_DONE_NAME "md5_semaphore_done"

typedef struct shmemData{
    char * name;
    int fd;
    size_t size;
    void * address;
} shmemData;

typedef struct semaphoreData{
    char * name;
    sem_t * semaphore;
} semaphoreData;

#endif