#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define BUFFER_SIZE 6

typedef struct{
    int in;
    int out;
    char element[BUFFER_SIZE][20];
}item;

int main(){
    const int SIZE = 4096;
    const char *name = "OS";
    char str[20];
    int shm_fd;
    item *ptr;
    
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(item));

    ptr = mmap(0, sizeof(item), PROT_WRITE, MAP_SHARED, shm_fd, 0);

    while(1){
        while((((ptr->in)+1)%BUFFER_SIZE)==ptr->out);
        fgets(str, sizeof(str), stdin);
        str[strlen(str) - 1] = '\0';
        strcpy(ptr->element[ptr->in], str);
        ptr->in = ((ptr->in) + 1) % BUFFER_SIZE;
    }

    return 0;
}