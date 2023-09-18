#include "./includes/slave.h"

int slaveProcess(int * masterToSlave, int * slaveToMaster){
    
    int slaveToSlaveMaster[2];
    char *params[] = {"md5sum", NULL, NULL};
    char * fileName;
    char output[MD5_LENGTH + 1] = {0};

    while(1){
        read(STDIN, &fileName, sizeof(char *));
        createPipe(slaveToSlaveMaster);
        if(createSlave() == 0){
            closePipe(slaveToSlaveMaster[STDIN]);
            dup2(slaveToSlaveMaster[STDOUT], STDOUT);
            close(slaveToSlaveMaster[STDOUT]);
            params[1] = fileName;
            execvp("md5sum", params);
        }
        else{
            close(slaveToSlaveMaster[STDOUT]);
            wait(NULL);
            read(slaveToSlaveMaster[STDIN], output, MD5_LENGTH);
            write(STDOUT, output, MD5_LENGTH);
        }
    }
    return 0;
}