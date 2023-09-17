#ifndef _SLAVE_H_
#define _SLAVE_H_

#include "utils.h"

#define MD5_LENGTH 32
#define STDIN 0
#define STDOUT 1
#define SLAVE_FILES 2

typedef struct slave{
    int masterToSlave[2];
    int slaveToMaster[2];
    char * name;
    int pid;
} slave;

int slaveProcess(int * appToSlave, int * slaveToApp);

#endif