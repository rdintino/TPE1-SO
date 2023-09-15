#include "./includes/main.h"

int main(int argc, char * argv[]){

    //Probando a ver si funciona bien la shmem
    const char *name = "/my_shared_memory";
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    size_t shm_size = 4096;
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("ftruncate");
        exit(1);
    }
    void *shm_ptr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    char *data = "Probando a lo boca";
    strncpy((char *)shm_ptr, data, strlen(data) + 1);

    if (munmap(shm_ptr, shm_size) == -1) {
        perror("munmap");
        exit(1);
    }

    close(shm_fd);

    char * files[argc - 1];
    int filesQty = 0;

    fd_set readFds, readFdsCopy;
    FD_ZERO(&readFds);
    readFdsCopy = readFds;

    if(argc <= 1){
        fprintf(stderr,"%s\n", "No files found");
        return -1;
    }

    for(int i = 1; i < filesQty; i++){
        if(isFile(argv[i])){
            files[filesQty++] = argv[i];
        }
    }

    int slavesQty = filesQty / SLAVE_FILES + 1;
    slave slaves[slavesQty];  

    for(int i = 0; i < slavesQty; i++){
        pipe(slaves[i].masterToSlave);
        pipe(slaves[i].slaveToMaster);
        FD_SET (slaves[i].slaveToMaster[0], &readFds);
    }

    int currentSlave = 0;
    int currentId;
    while(currentSlave < slavesQty){
        if((currentId = createSlave()) == 0){
            break;
        }
        currentSlave++;
    }

    if(currentId == 0){
        closePipe(slaves[currentSlave-1].masterToSlave[1]);
        closePipe(slaves[currentSlave-1].slaveToMaster[0]);

        slaveProcess(slaves[currentSlave-1].masterToSlave, slaves[currentSlave-1].slaveToMaster);
    }

    else{
        for(int i = 0; i < slavesQty; i++){
            closePipe(slaves[i].masterToSlave[0]);
            closePipe(slaves[i].slaveToMaster[1]);
        }
        int currentFile = 0;
        for(int i = 0; i < slavesQty && currentFile < filesQty; i++, currentFile++){
            write(slaves[i].masterToSlave[1], files[currentFile], sizeof(char *));
            slaves[i].name = files[currentFile];
        }
        char hash[MD5_LENGTH + 1] = {0};
        int filesRead = 0;
        while(filesRead < filesQty){
            if(select(slavesQty, &readFds, NULL, NULL, NULL) == -1){
                perror("Problem with select - Master\n");
                exit(1);
            }
            for(int i = 0; i < slavesQty; i++){
                if(FD_ISSET(slaves[i].slaveToMaster[0], &readFds)){
                    if(read(slaves[i].slaveToMaster[0], hash, MD5_LENGTH + 1) == -1){
                        perror("Problem with read - Master\n");
                        exit(1);
                    }
                    //buffer implementation here

                    filesRead++;
                }
            }
            readFds = readFdsCopy;
    }

    printf("llegamos\n");
    }
}
