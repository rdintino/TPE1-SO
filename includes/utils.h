#ifndef _UTILS_H_
#define _UTILS_H_

#include "ipc.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MD5_LENGTH 32

typedef struct hashData{
    int pid;
    char hash[MD5_LENGTH + 1];
    char file[256];
    int isFinished;
} hashData;
int isFile(char * path);
void createPipe(int fd[2]);
int createSlave();
void closePipe(int fd);
void dupPipe(int fd1, int fd2);
FILE * openFile(char * path, char * mode);
void createSharedMemory(shmemData * shmem);
void closeSharedMemory(shmemData * shmem);
void writeInSharedMemory(int fd, const void *buf, size_t length, int offset);
void unlinkSharedMemory(char * name);
#endif