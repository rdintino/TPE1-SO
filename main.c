#include "./includes/main.h"


int main(int argc, char * argv[]){

    char * files[argc];
    int filesQty = 0;

    if(argc <= 1){
        fprintf(stderr,"%s\n", "No files found");
        return -1;
    }

    for(int i = 0; i < argc; i++ ){   //not sure if i starts at 0
        files[filesQty++] = argv[i];
    }

    int slavesQty = filesQty / SLAVE_FILES + 1;
    slave slaves[slavesQty];  

    for(int i = 0; i < slavesQty; i++){
        pipe(slaves[i].masterToSlave);
        pipe(slaves[i].slaveToMaster);
    }

    int currentSlave;
    int currentId = 1;
    while(currentSlave < slavesQty && currentId != 0){
        currentId = fork();
        if(currentId == 0){
            break;
        }
        currentSlave++;
    }

    if(currentId == 0){
        close(slaves[currentSlave-1].masterToSlave[1]);
        close(slaves[currentSlave-1].slaveToMaster[0]);

        slaveProcess(slaves[currentSlave-1].masterToSlave, slaves[currentSlave-1].slaveToMaster);
    }

    printf("llegamos\n");
}