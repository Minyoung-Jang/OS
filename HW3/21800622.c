#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define SIZE 1024

struct message{
    int type;
    char text[SIZE];
};

int repeat_receiver = 1;
struct message msg;
char temp[SIZE] = "";

void *msg_sender(void *param){

    while(strcmp(temp, "quit") != 0){
        printf("[mesg]  ");
        scanf("%s", temp);
        strcpy(msg.text, temp);

        if (strcmp(temp, "quit") == 0){
            repeat_receiver = 0;
            pthread_exit(0);
        }else{
            int result = msgsnd((int)param, &msg, sizeof(msg) - sizeof(int), 0);
            if(result == -1){
                perror("Failed to send the message");
                exit(1);
            }
        }
    }
}

void *msg_receiver(void *param){

    while(repeat_receiver == 1){
        int result = msgrcv((int)param, &msg, (sizeof(msg) - sizeof(int)), 1, IPC_NOWAIT);
        if(result == -1){
            perror("Failed to receive the message");
            exit(1);
        }else{
            printf("                [incoming] \"%s\"", msg.text);
        }
    }
    
}

int main(int argc, char *argv[]){

    int send_qid = 0, receive_qid = 0;
    pthread_t send_tid, receive_tid;
    pthread_attr_t send_attr, receive_attr;

    if(argc < 3){
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
    send_qid = msgget((key_t)atoi(argv[1]), IPC_CREAT | 0666);
    receive_qid = msgget((key_t)atoi(argv[2]), IPC_CREAT | 0666);

    // Get the Default Attributes
    pthread_attr_init(&send_attr);
    pthread_attr_init(&receive_attr);

    // Create Message Threads
    pthread_create(&send_tid, &send_attr, msg_sender, &send_qid);
    pthread_create(&receive_tid, &receive_attr, msg_sender, &receive_qid);

    // Wait for the Threads
    pthread_join(send_tid, NULL);
    pthread_join(receive_tid, NULL);

    // Deallocate the Queues
    msgctl(send_qid, IPC_RMID, 0);
    msgctl(receive_qid, IPC_RMID, 0);

    return 0;
}