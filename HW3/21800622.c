#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define SIZE 1024
#define TYPE 1

typedef struct{
    long type;
    char msg[SIZE];
} Message;

int repeat_receiver = 1;
char string_buffer[SIZE];
Message message;
int flag = 0;

void *msg_sender(void *param);
void *msg_receiver(void *param);

int main(int argc, char *argv[]){
    int send_queue = 0, receive_queue = 0;
    pthread_t send_tid, receive_tid;
    pthread_attr_t send_attr, receive_attr;

    if(argc != 3){
        fprintf(stderr, "usage: ./hw3 <snd_key> <rcv_key>\n");
        exit(0);
    }

    if(atoi(argv[1]) < 0 || atoi(argv[2]) < 0){
        if(atoi(argv[1]) < 0){
            fprintf(stderr, "%d must be >= 0\n", atoi(argv[1]));
        }else{
            fprintf(stderr, "%d must be >= 0\n", atoi(argv[2]));
        }
        exit(0);
    }

    // Create Message Queue ID
    send_queue = msgget((key_t)atoi(argv[1]), IPC_CREAT | 0666);
    receive_queue = msgget((key_t)atoi(argv[2]), IPC_CREAT | 0666);

    // Get the Default Attributes
    pthread_attr_init(&send_attr);
    pthread_attr_init(&receive_attr);

    // Create Message Threads
    pthread_create(&send_tid, &send_attr, msg_sender, &send_queue);
    pthread_create(&receive_tid, &receive_attr, msg_receiver, &receive_queue);

    // Wait for the Threads
    pthread_join(send_tid, NULL);
    pthread_join(receive_tid, NULL);

    // Deallocate the Queues
    msgctl(send_queue, IPC_RMID, 0);
    msgctl(receive_queue, IPC_RMID, 0);

    return 0;
}

void *msg_sender(void *param){
    while(strcmp(string_buffer, "quit") != 0){
        printf("[msg] ");
        fgets(string_buffer, SIZE , stdin);

        if (strcmp(string_buffer, "\n") == 0) continue;

        string_buffer[strlen(string_buffer)-1] = '\0';
        message.type = TYPE;
        strcpy(message.msg, string_buffer);

        if (strcmp(string_buffer, "quit") == 0){
            repeat_receiver = 0;
            pthread_exit(0);
        }

        int result = msgsnd(*(int*)param, &message, sizeof(Message)-sizeof(long), 0);

        if(result == -1){
            perror("Failed to send the message\n");
            exit(1);
        }
    }        
    return NULL;
}

void *msg_receiver(void *param){
    while(repeat_receiver == 1){
        int result = msgrcv(*(int*)param, &message, sizeof(Message) - sizeof(long), TYPE, IPC_NOWAIT);
        if(result != -1){
            printf("\t\t\t[incoming] \"%s\"", message.msg);
        }   
    }
    pthread_exit(0);
}