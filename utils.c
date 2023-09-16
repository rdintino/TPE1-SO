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
    int shm_fd = shm_open(shmem->name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("Problem creating shared memory");
        exit(1);
    }
    if (ftruncate(shm_fd, shmem->size) == -1) {
        perror("Problem resizing shared memory");
        exit(1);
    }
    shmem->fd = shm_fd;
    shmem->address = mmap(NULL, shmem->size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shmem->address == MAP_FAILED) {
        perror("Problem mapping shared memory");
        exit(1);
    }
}

void closeSharedMemory(shmemData * shmem){
    if(munmap(shmem->address, shmem->size) == -1){
        perror("Problem unmapping shared memory");
        exit(1);
    }
    closePipe(shmem->fd);
    unlinkSharedMemory(shmem->name);
}

void writeInSharedMemory(int fd, const void *buf, size_t length, int offset){
    if(pwrite(fd, buf, length, length * offset) == -1){
        perror("Problem writing in shared memory");
        exit(1);
    }
}

void unlinkSharedMemory(char * name){
    if(shm_unlink(name) == -1){
        perror("Problem unlinking shared memory");
        exit(1);
    }
}