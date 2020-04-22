#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

typedef struct
{
    long type;
    char message[BUFFER_SIZE];
} s_data;

int repeat_receiver = 1;
char string_buffer[BUFFER_SIZE];
int num = 0;
s_data data;

void *receiver(void *);
void *sender(void *);

int main(int argc, char *argv[])
{
    int sqid, rqid;
    pthread_t stid, rtid;
    pthread_attr_t sattr, rattr;

    if (argc < 3)
    {
        fprintf(stderr, "Usage: ./hw3 <snd_key> <rcv_key>\n");
        exit(0);
    }
    if (atoi(argv[1]) < 0 || atoi(argv[2]) < 0)
    {
        fprintf(stderr, "x should be natural number\n");
        exit(0);
    }

    sqid = msgget((key_t)atoi(argv[1]), IPC_CREAT | 0666);
    if (sqid == -1)
    {
        perror("msgget error : ");
        exit(0);
    }

    rqid = msgget((key_t)atoi(argv[2]), IPC_CREAT | 0666);
    if (rqid == -1)
    {
        perror("msgget error : ");
        exit(0);
    }

    pthread_attr_init(&sattr);
    pthread_attr_init(&rattr);

    pthread_create(&stid, &sattr, sender, &sqid);
    pthread_create(&rtid, &rattr, receiver, &rqid);

    pthread_join(stid, NULL);
    pthread_join(rtid, NULL);

    msgctl(sqid, IPC_RMID, 0);
    msgctl(rqid, IPC_RMID, 0);

    return 0;
}

void *sender(void *param)
{
    int ret = 0;

    while (strcmp(string_buffer, "quit") != 0)
    {
        fflush(stdout);
        printf("[msg] ");
        fgets(string_buffer, BUFFER_SIZE, stdin);

        if (strcmp(string_buffer, "\n") == 0)
            continue;

        string_buffer[strlen(string_buffer) - 1] = '\0';
        data.type = 1;
        strcpy(data.message, string_buffer);

        if (strcmp(string_buffer, "quit") == 0)
        {
            repeat_receiver = 0;
            pthread_exit(0);
        }

        ret = msgsnd(*(int *)param, &data, sizeof(s_data) - sizeof(long), 0);

        if (ret == -1)
        {
            printf("error\n");
            perror("msgsnd() 실패");
            exit(1);
        }
    }
    fflush(stdout);
    return NULL;
}

void *receiver(void *param)
{
    fflush(stdout);

    while (repeat_receiver == 1)
    {

        int ret = 0;

        ret = msgrcv(*(int *)param, &data, (sizeof(s_data) - sizeof(long)), 1, IPC_NOWAIT);

        if (ret != -1)
            printf("               [incomming] %s\n[msg] ", data.message);

        fflush(stdout);
        usleep(1000);
    }
    fflush(stdout);
    pthread_exit(0);
}