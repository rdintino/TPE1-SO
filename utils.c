#include "./includes/utils.h"

int isFile(char * path){
    struct stat pathStat;
    stat(path, &pathStat);
    return S_ISREG(pathStat.st_mode);
}

FILE * openFile(char * path, char * mode){
    FILE * file = fopen(path, mode);
    if(file == NULL){
        error("Problem opening file", FILE_ERROR);
    }
    return file;
}

void createPipe(int fd[2]) {
    if(pipe(fd) == -1) {
        error("Problem creating a pipe", PIPE_ERROR);
    }
}

int createSlave(){
    int id;
    if((id = fork()) == -1){
        error("Problem creating a slave", SLAVE_ERROR);
     }
    return id;
}

void closePipe(int fd){
    if(close(fd) == -1){
        error("Problem closing a pipe", PIPE_ERROR);
    }
}

void dupPipe(int fd1, int fd2){
    if(dup2(fd1, fd2) == -1){
        error("Problem duplicating a pipe", PIPE_ERROR);
    }
}

void createSharedMemory(shmemData * shmem){
    shmem->name = SHMEM_NAME;
    shmem->size = SHMEM_SIZE;
    int shmem_fd = shm_open(shmem->name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (shmem_fd == -1) {
        error("Problem creating shared memory", SHMEM_ERROR);
    }
    if (ftruncate(shmem_fd, shmem->size) == -1) {
        error("Problem truncating shared memory", SHMEM_ERROR);
    }
    shmem->fd = shmem_fd;
    shmem->address = mmap(NULL, shmem->size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
    if (shmem->address == MAP_FAILED) {
        error("Problem mapping shared memory", SHMEM_ERROR);
    }
}

void openIPC(shmemData * shmem, semaphoreData * semaphoreRead, semaphoreData * semaphoreDone){
    if((shmem->fd = shm_open(shmem->name, O_RDONLY, S_IRUSR)) == -1){
        error("Problem opening shared memory", SHMEM_ERROR);
    }
    if((shmem->address = mmap(NULL, shmem->size, PROT_READ, MAP_SHARED, shmem->fd, 0)) == MAP_FAILED){
        error("Problem mapping shared memory", SHMEM_ERROR);
    }
    openSemaphore(semaphoreRead);
    openSemaphore(semaphoreDone);
}

void closeSharedMemory(shmemData * shmem){
    if(munmap(shmem->address, shmem->size) == -1){
        error("Problem unmapping shared memory", SHMEM_ERROR);
    }
    closePipe(shmem->fd);
}

void writeInSharedMemory(int fd, const void *buf, size_t size, int offset){
    if(pwrite(fd, buf, size, size * offset) == -1){
        error("Problem writing in shared memory", SHMEM_ERROR);
    }
}

void readFromSharedMemory(shmemData *shmem, const void *buf, size_t size, int offset){
    if(pread(shmem->fd, buf, size, size * offset) == -1){
        error("Problem reading from shared memory", SHMEM_ERROR);
    }
}

void unlinkSharedMemory(char * name){
    if(shm_unlink(name) == -1){
        error("Problem unlinking shared memory", SHMEM_ERROR);
    }
}

void createSemaphore(semaphoreData * semaphore, char * name){
    semaphore->name = name;
    if((semaphore->semaphore = sem_open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED ){
        unlinkSharedMemory(semaphore->name);
        error("Problem creating semaphore", SEMAPHORE_ERROR);
    }
}

void openSemaphore(semaphoreData * semaphore){
    if((semaphore->semaphore = sem_open(semaphore->name, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
        error("Problem opening semaphore", SEMAPHORE_ERROR);
    }
}

void closeSemaphore(sem_t * semaphore){
    if(sem_close(semaphore) == -1){
        error("Problem closing semaphore", SEMAPHORE_ERROR);
    }
}

void unlinkSemaphore(char * name){
    if(sem_unlink(name) == -1){
        error("Problem unlinking semaphore", SEMAPHORE_ERROR);
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

void error(char * msg, int code) {
    perror(msg);
    exit(code);
}
