#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int isFile(char * path);
void createPipe(int fd[2]);
int createSlave();
void closePipe(int fd);
void dupPipe(int fd1, int fd2);
#endif