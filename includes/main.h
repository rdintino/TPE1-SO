#ifndef _MAIN_H_
#define _MAIN_H_

#include "slave.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <string.h>

#define SLAVE_FILES 2

typedef struct slave{
    int masterToSlave[2];
    int slaveToMaster[2];
    char * name;
    int pid;
} slave;

#endif