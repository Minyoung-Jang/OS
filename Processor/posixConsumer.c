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
    char str[20] = "";
    char key[20] = "";
    int shm_fd;
    item *ptr;
    
    shm_fd = shm_open(name, O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(item));

    ptr = mmap(0, sizeof(item), PROT_WRITE, MAP_SHARED, shm_fd, 0);

    while(1){
        while(ptr->in==ptr->out);
        fgets(key, sizeof(key), stdin);
        key[strlen(key) - 1] = '\0';

        if(strcmp(key, "go") == 0){
            strcpy(str, ptr->element[ptr->out]);
            ptr->out = ((ptr->out)+1)%BUFFER_SIZE;
        }
        printf("%s\n",str);
    }

    shm_unlink(name);
    return 0;
}