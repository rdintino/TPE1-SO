#include "./includes/slave.h"

int slaveProcess(int * masterToSlave, int * slaveToMaster){
    
    int slaveToSlaveMaster[2];
    char *params[] = {"/usr/bin/md5sum", NULL, NULL};
    char * fileName;
    char output[MD5_LENGTH + 1] = {0};

    while(1){
        read(masterToSlave[STDIN], &fileName, sizeof(char *));
        createPipe(slaveToSlaveMaster);
        if(createSlave() == 0){
            closePipe(STDIN);
            closePipe(slaveToSlaveMaster[STDIN]);
            dupPipe(slaveToSlaveMaster[STDOUT], STDOUT);
            closePipe(slaveToSlaveMaster[STDOUT]);
            params[1] = fileName;
            execvp("md5sum", params);
        }
        else{
            closePipe(slaveToSlaveMaster[STDOUT]);
            dupPipe(slaveToSlaveMaster[STDIN], STDIN);
            closePipe(slaveToSlaveMaster[STDIN]);
            wait(NULL);
            read(STDIN, output, MD5_LENGTH);
            write(slaveToMaster[STDOUT], output, MD5_LENGTH);        }
    }
    return 0;
}