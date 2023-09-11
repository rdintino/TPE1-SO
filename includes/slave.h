#ifndef _SLAVE_H_
#define _SLAVE_H_

#define MD5_LENGTH 32
#define STDIN 0
#define STDOUT 1

#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include<sys/wait.h>
#include <string.h>

int slaveProcess(int * appToSlave, int * slaveToApp);

#endif