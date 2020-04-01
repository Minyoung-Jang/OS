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
    const char *message_0 = "Hello";
    const char *message_1 = "World!";

    int shm_fd;

    void *ptr;
    
    shm_fd = shmget(IPC_PRIVATE, SIZE, S_IRUSR | S_IWUSR);
    ptr = shmat(shm_fd, NULL, 0);

    sprintf(ptr, "%s", message_0);
    printf("%s\n", ptr);
    ptr += strlen(message_0);
    sprintf(ptr,"%s",message_1);
    printf("%s\n", ptr);
    ptr += strlen(message_1);

    return 0;
}