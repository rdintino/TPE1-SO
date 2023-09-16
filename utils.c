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