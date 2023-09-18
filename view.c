#include "./includes/view.h"

int main(int argc, char *argv[]) {
    char shmemName[BUFFER_SIZE], semaphoreReadName[BUFFER_SIZE], semaphoreDoneName[BUFFER_SIZE];
    shmemData shmem;
    semaphoreData semaphoreRead, semaphoreDone;
    hashData buffer;
    if (argc >= 4) {
        strncpy(shmemName, argv[1], BUFFER_SIZE);
        strncpy(semaphoreReadName, argv[2], BUFFER_SIZE);
        strncpy(semaphoreDoneName, argv[3], BUFFER_SIZE);
    } else {
        // fgets(shmemName, BUFFER_SIZE, stdin);
        // int shmemNameLength = strlen(shmemName);
        // fgets(semaphoreReadName, BUFFER_SIZE, stdin);
        // int semaphoreReadNameLength = strlen(semaphoreReadName);
        // fgets(semaphoreDoneName, BUFFER_SIZE, stdin);
        // int semaphoreDoneNameLength = strlen(semaphoreDoneName);
        // if(shmemNameLength == 0 || semaphoreReadNameLength == 0 || semaphoreDoneNameLength == 0) {
        //     perror("Invalid params");
        //     exit(1);
        // }
    
        // shmemName[shmemNameLength - 1] = 0;
        // semaphoreReadName[semaphoreReadNameLength - 1] = 0;
        // semaphoreDoneName[semaphoreDoneNameLength - 1] = 0;
        scanf("%s", shmemName);
        scanf("%s", semaphoreReadName);
        scanf("%s", semaphoreDoneName);
    }
        shmem.name = shmemName;
        shmem.size = SHMEM_SIZE;
        semaphoreRead.name = semaphoreReadName;
        semaphoreDone.name = semaphoreDoneName;

        openIPC(&shmem, &semaphoreRead, &semaphoreDone);
        sem_wait(semaphoreDone.semaphore);
        setvbuf(stdout, NULL, _IONBF, 0);

        int done = 0;
        int offset = 0;
        while(!done){
            sem_wait(semaphoreRead.semaphore);
            readFromSharedMemory(&shmem, &buffer, sizeof(buffer), offset);
            offset++;
            printf("PID: %d, HASH: %s, FILE: %s\n", buffer.pid, buffer.hash, buffer.file);
            if(buffer.isFinished){
                done = 1;
            }
        }
    sem_post(semaphoreDone.semaphore);
    closeIPC(&shmem, &semaphoreRead, &semaphoreDone);
    return 0;
}