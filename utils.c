#include "./includes/utils.h"

int isFile(char * path){
    struct stat pathStat;
    stat(path, &pathStat);
    return S_ISREG(pathStat.st_mode);
}

FILE * openFile(char * path, char * mode){
    FILE * file = fopen(path, mode);
    if(file == NULL){
        perror("Problem opening file");
        exit(1);
    }
    return file;
}

void createPipe(int fd[2]) {
    if(pipe(fd) == -1) {
        perror("Problem creating a pipe");
        exit(1);
    }
}

int createSlave(){
    int id;
    if((id = fork()) == -1){
        perror("Problem creating a slave");
        exit(1);
     }
    return id;
}

void closePipe(int fd){
    if(close(fd) == -1){
        perror("Problem closing a pipe");
        exit(1);
    }
}

void dupPipe(int fd1, int fd2){
    if(dup2(fd1, fd2) == -1){
        perror("Problem duplicating a pipe");
        exit(1);
    }
}

void createSharedMemory(shmemData * shmem){
    shmem->name = SHMEM_NAME;
    shmem->size = SHMEM_SIZE;
    int shmem_fd = shm_open(shmem->name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (shmem_fd == -1) {
        perror("Problem creating shared memory");
        exit(1);
    }
    if (ftruncate(shmem_fd, shmem->size) == -1) {
        perror("Problem resizing shared memory");
        exit(1);
    }
    shmem->fd = shmem_fd;
    shmem->address = mmap(NULL, shmem->size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
    if (shmem->address == MAP_FAILED) {
        perror("Problem mapping shared memory while creating");
        exit(1);
    }
}

void openIPC(shmemData * shmem, semaphoreData * semaphoreRead, semaphoreData * semaphoreDone){
    if((shmem->fd = shm_open(shmem->name, O_RDONLY, S_IRUSR)) == -1){
        perror("Problem opening shared memory");
        exit(1);
    }
    if((shmem->address = mmap(NULL, shmem->size, PROT_READ, MAP_SHARED, shmem->fd, 0)) == MAP_FAILED){
        perror("Problem mapping shared memory while opening");
        exit(1);
    }
    openSemaphore(semaphoreRead);
    openSemaphore(semaphoreDone);
    sem_wait(semaphoreDone->semaphore);
}

void closeSharedMemory(shmemData * shmem){
    if(munmap(shmem->address, shmem->size) == -1){
        perror("Problem unmapping shared memory");
        exit(1);
    }
    closePipe(shmem->fd);
}

void writeInSharedMemory(int fd, const void *buf, size_t size, int offset){
    if(pwrite(fd, buf, size, size * offset) == -1){
        perror("Problem writing in shared memory");
        exit(1);
    }
}

void readFromSharedMemory(shmemData *shmem, const void *buf, size_t size, int offset){
    if(pread(shmem->fd, buf, size, size * offset) == -1){
        perror("Problem reading from shared memory");
        exit(1);
    }
}

void unlinkSharedMemory(char * name){
    if(shm_unlink(name) == -1){
        perror("Problem unlinking shared memory");
        exit(1);
    }
}

void createSemaphore(semaphoreData * semaphore, char * name){
    semaphore->name = name;
    if((semaphore->semaphore = sem_open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED ){
        unlinkSharedMemory(semaphore->name);
        perror("Problem creating semaphore");
        exit(1);
    }
}

void openSemaphore(semaphoreData * semaphore){
    if((semaphore->semaphore = sem_open(semaphore->name, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
        perror("Problem opening semaphore");
        exit(1);
    }
}

void closeSemaphore(sem_t * semaphore){
    if(sem_close(semaphore) == -1){
        perror("Problem closing semaphore");
        exit(1);
    }
}

void unlinkSemaphore(char * name){
    if(sem_unlink(name) == -1){
        perror("Problem unlinking semaphore");
        exit(1);
    }
}

void closeIPC(shmemData * shmem, semaphoreData * semaphoreRead, semaphoreData * semaphoreDone){
    closeSharedMemory(shmem);
    closeSemaphore(semaphoreRead->semaphore);
    closeSemaphore(semaphoreDone->semaphore);
}

void closeApplication(shmemData * shmem, semaphoreData * semaphoreRead, semaphoreData * semaphoreDone, FILE * file){
    closeIPC(shmem, semaphoreRead, semaphoreDone);
    fclose(file);
    unlinkSharedMemory(shmem->name);
    unlinkSemaphore(semaphoreRead->name);
    unlinkSemaphore(semaphoreDone->name);
}