#ifndef _MAIN_H_
#define _MAIN_H_

#include "slave.h"

#define SLAVE_FILES 2

typedef struct slave{
    int masterToSlave[2];
    int slaveToMaster[2];
} slave;

#endif