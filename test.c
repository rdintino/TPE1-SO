#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


int main() {
    const char *name = "/shmem";
    int shm_fd = shm_open(name, O_RDWR, 0);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    size_t shm_size = 4096;

    void *shm_ptr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    printf("Imprimiendo shmem: %s\n", (char *)shm_ptr);

    if (munmap(shm_ptr, shm_size) == -1) {
        perror("munmap");
        exit(1);
    }

    // Close the shared memory object
    close(shm_fd);

    return 0;
}