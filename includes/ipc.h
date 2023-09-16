#ifndef _IPC_H_
#define _IPC_H_

#include <semaphore.h>
#include <sys/mman.h>

#define SHMEM_SIZE 4096
#define SHMEM_NAME "md5_shmem"
#define SEM_READ "sem_read"
#define SEM_CLOSE "sem_close"

typedef struct shmemData{
    char * name;
    int fd;
    size_t size;
    void * address;
} shmemData;

#endif