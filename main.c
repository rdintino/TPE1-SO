#include "./includes/main.h"

int main(int argc, char * argv[]){
    char * files[argc];
    int filesQty = 0;
    FILE * file = openFile("resultado.txt", "w");

    fd_set readFds, readFdsCopy;
    FD_ZERO(&readFds);

    if(argc <= 1){
        error("No files were given", FILE_ERROR);
    }
    
    for(int i = 1; i < argc; i++){
        if(isFile(argv[i])){
            files[filesQty++] = argv[i];
        }
    }
    //Creation of pipes
    int slavesQty = filesQty / SLAVE_FILES + 1;
    slave slaves[slavesQty];  
    for(int i = 0; i < slavesQty; i++){
        createPipe(slaves[i].masterToSlave);
        createPipe(slaves[i].slaveToMaster);
        FD_SET (slaves[i].slaveToMaster[STDIN], &readFds);
    }
    readFdsCopy = readFds; //To be restored after select
    //Creation of shared memory
    shmemData shmem;
    createSharedMemory(&shmem);
    //Creation of semaphores
    semaphoreData semaphoreRead, semaphoreDone;
    createSemaphore(&semaphoreRead, SEM_READ_NAME);
    createSemaphore(&semaphoreDone, SEM_DONE_NAME);

    sem_post(semaphoreDone.semaphore);
    setvbuf(stdout, NULL, _IONBF, 0);
    //To be restored in view
    printf("%s\n", SHMEM_NAME);
    printf("%s\n", SEM_READ_NAME);
    printf("%s\n", SEM_DONE_NAME);
    
    sleep(2);
    //Creation of slaves
    int currentSlave = 0;
    int currentId = 1;
    while(currentSlave < slavesQty && currentId != 0){
        currentId = createSlave();
        slaves[currentSlave++].pid = currentId;
    }
    //CHILD PROCESS
    if(currentId == 0){
        closePipe(slaves[currentSlave-1].masterToSlave[STDOUT]);
        closePipe(slaves[currentSlave-1].slaveToMaster[STDIN]);
        slaveProcess(slaves[currentSlave-1].masterToSlave, slaves[currentSlave-1].slaveToMaster);
    } 
    //PARENT PROCESS
    else{
        for(int i = 0; i < slavesQty; i++){
            closePipe(slaves[i].masterToSlave[STDIN]);
            closePipe(slaves[i].slaveToMaster[STDOUT]);
        }
        int currentFile = 0;
        for(int i = 0; currentFile < slavesQty; i++){
            write(slaves[i].masterToSlave[STDOUT], &(files[currentFile]), sizeof(char *));
            slaves[i].name = files[currentFile++];
        }
        char hash[MD5_LENGTH + 1] = {0};
        hashData buffer;
        memset(&buffer, 0, sizeof(buffer));
        int filesRead = 0;
        while(filesRead < filesQty){
            if(select(FD_SETSIZE, &readFds, NULL, NULL, NULL) == -1){
                perror("Problem with select");
                exit(1);
            }
            for(int i = 0; i < slavesQty && filesRead < filesQty; i++){
                if(FD_ISSET(slaves[i].slaveToMaster[STDIN], &readFds)){
                    if(read(slaves[i].slaveToMaster[STDIN], hash, MD5_LENGTH + 1) == -1){
                        error("Problem reading from pipe", PIPE_ERROR);
                    }
                    buffer.pid = slaves[i].pid;
                    strcpy(buffer.hash, hash);
                    strcpy(buffer.file, slaves[i].name);
                    if(filesQty - filesRead <= 1){
                        buffer.isFinished = 1;
                    }
                    //Writing file data to resultado.txt
                    fprintf(file, "PID: %d, HASH: %s, FILE: %s\n", buffer.pid, buffer.hash, buffer.file);
                    //Writing file data to shared memory
                    writeInSharedMemory(shmem.fd, &buffer, sizeof(hashData), filesRead);
                    sem_post(semaphoreRead.semaphore);
                    filesRead++;
                    if(currentFile < filesQty){
                        write(slaves[i].masterToSlave[1], &(files[currentFile]), sizeof(char *));
                        slaves[i].name = files[currentFile++];
                    }
                }
            }
            readFds = readFdsCopy;
        }
        for(int i = 0; i < slavesQty; i++){
            closePipe(slaves[i].masterToSlave[STDOUT]);
            closePipe(slaves[i].slaveToMaster[STDIN]);
            kill(slaves[i].pid, SIGKILL);
        }
        closeApplication(&shmem, &semaphoreRead, &semaphoreDone, file);
    }
    return 0;
}