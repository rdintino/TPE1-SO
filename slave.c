#include "./includes/slave.h"

int slaveProcess(int * masterToSlave, int * slaveToMaster){
    
    int slaveToSlaveMaster[2];
    char *params[] = {"/usr/bin/md5sum", NULL};
    char output[MD5_LENGTH + 1] = {0};

    if(pipe(slaveToSlaveMaster) == 0){
        if(fork() == 0){
            close(0);
            close(slaveToSlaveMaster[STDIN]);
            dup2(slaveToSlaveMaster[STDOUT],1);
            close(slaveToSlaveMaster[STDOUT]);
            execvp("/usr/bin/md5sum",params);  // TRIAL   
        }
        else{
            wait(NULL);
            close(slaveToSlaveMaster[STDOUT]);
            dup2(slaveToSlaveMaster[STDIN], 0);
            close(slaveToSlaveMaster[STDIN]);
            read(0, output, MD5_LENGTH + 1);
            printf("\n MD5 : %s\n", output);
        }
    }
    return 0;
}