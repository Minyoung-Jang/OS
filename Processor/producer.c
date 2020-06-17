  
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

//gcc sharedProducer.c -o p -lrt
//gcc sharedConsumer.c -o c -lrt

//-lrt


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
    char chat[20];

    int shm_fd;

    entire *ptr;
    
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(entire));
    //printf("%ld\n",sizeof(entire));

    ptr = mmap(0,sizeof(entire), PROT_WRITE, MAP_SHARED,  shm_fd,0);
    ptr->in = 0;
    ptr->out = 0;
    

    //printf("%d\n",ptr->in);

    do{

    

        // ptr->flag[0]=true;
        // ptr->turn=1;
        while((((ptr->in)+1)%BUFFER_SIZE)==ptr->out);
        
        // while(ptr->flag[1] && ptr-> turn==1 );

        fgets(chat,sizeof(chat),stdin);
        chat[strlen(chat)-1]='\0';
        
        ptr->flag[0]=true;
        ptr->turn=1;
        while(ptr->flag[1] && ptr-> turn==1 );
        strcpy(ptr->item[ptr->in],chat);
        if(strcmp(chat,"exit")==0){
            munmap(ptr,sizeof(entire));
            break;
        }
        ptr->in = ((ptr->in)+1)%BUFFER_SIZE;
        //printf("%n",ptr->item[(ptr->in)-1]);
        printf("in : %d   out : %d\n",ptr->in,ptr->out);
        ptr->flag[0]=false;
    
        fflush(stdout);


    }while(1);

    
    shm_unlink(name);

    return 0;
}