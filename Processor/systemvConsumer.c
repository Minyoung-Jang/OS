#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(){

    const int SIZE = 4096;
    const char *name = "OS";

    int shm_fd;
    void *ptr;
    
    shm_fd = shmget(IPC_PRIVATE, SIZE, S_IRUSR | S_IWUSR);
    ptr = shmat(shm_fd, NULL, 0);
    
    printf("%s\n",(char*)ptr);

    shmdt(ptr);
    shmctl(shm_fd, IPC_RMID, NULL);

    return 0;
}
