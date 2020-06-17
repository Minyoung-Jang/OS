#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>


#define BUFFER_SIZE 6



typedef struct{
    int in;
    int out;
    bool turn;
    bool flag[2];
    char item[6][20];
}entire;

int main(){

    const int SIZE = 4096;
    const char *name = "OSS";
    char chat[20]="";
    char ch[20];
    int shm_fd;
    bool check=false;

    entire *ptr;
    
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(entire));

    ptr = mmap(0,sizeof(entire), PROT_WRITE, MAP_SHARED,  shm_fd,0);
   

    do{
        // ptr->flag[1]=true;
        // ptr->turn=0;
        while(ptr->in==ptr->out);
        //if(check==true) break;

        // while(ptr->flag[0] && ptr->turn==0 );

        //critical part

        fgets(ch,sizeof(ch),stdin);
        ch[strlen(ch)-1]='\0';

        while(ptr->flag[0] && ptr->turn==0);
        
        if(strcmp(ch,"exit")==0){
            munmap(ptr,sizeof(entire));
            break;
        }

        if(strcmp(ch,"go")==0){
            ptr->flag[1]=true;
            ptr->turn=0;
            strcpy(chat,ptr->item[ptr->out]);
            ptr->out = ((ptr->out)+1)%BUFFER_SIZE;
            printf("in: %d    out: %d     component: %s\n",ptr->in,ptr->out,chat);
        }
        
        ptr->flag[1]=false;

        // ptr->flag[1]=false;

        //remainter part
        fflush(stdout);


    }while(1);

    shm_unlink(name);

    return 0;
}