#ifndef _SLAVE_H_
#define _SLAVE_H_

#define MD5_LENGTH 32
#define STDIN 0
#define STDOUT 1

#include "utils.h"

int slaveProcess(int * appToSlave, int * slaveToApp);

#endif