#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <sys/ipc.h>
// #include <sys/types.h>
// #include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define MAX_MESSAGE 64
char buffer[BUFFER_SIZE][MAX_MESSAGE];
int in = 0, out = 0, count = 0;

pthread_mutex_t mutex;
sem_t empty, full;
int empty_num, full_num;

int repeat = 1;

void DisplayBuffer();
void *Producer();
void *Consumer();

int main(int argc, char *argv[])
{

    int duration;
    if (argc < 2)
        duration = 30;

    else
        duration = atoi(argv[1]);

    srand(time(NULL));

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    DisplayBuffer();

    pthread_t pro, con;
    pthread_attr_t pro_attr, con_attr;

    pthread_attr_init(&pro_attr);
    pthread_attr_init(&con_attr);

    pthread_create(&pro, &pro_attr, Producer, NULL);
    pthread_create(&con, &con_attr, Consumer, NULL);

    sleep(duration);

    repeat = 0;
    sem_getvalue(&full, &full_num);
    sem_getvalue(&empty, &empty_num);

    if (full_num == 0)
    {
        printf("%d full!!!!!!!!!!!\n", full_num);
        sem_post(&full);
    }
    if (empty_num == 0)
    {
        printf("%d empty!!!!!!!!!!\n", empty_num);
        sem_post(&empty);
    }

    pthread_join(pro, NULL);
    pthread_join(con, NULL);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&empty);

    printf("Bye!\n");
}

void *Producer()
{
    int no_messages = 10;
    char *messages[64] = {
        "Nice to see you!",
        "Aal izz well!",
        "I love you! God loves you!",
        "God loves you and has a wonderful plan for your life.", "God bless you!",
        "You are genius!",
        "Cheer up!",
        "Everything is gonna be okay.",
        "You are so precious!",
        "Hakuna matata!"};
    do
    {
        sleep(rand() % 3 + 1);
        char item[MAX_MESSAGE];
        int num = rand() % 10;
        strcpy(item, messages[num]);
        printf("[Producer] Created a message \"%s \" \n", item);

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        printf("---------------------- PRODUCER ------------------------\n");
        printf("Producer is entering critical section.\n");
        strcpy(buffer[in], item);
        printf("[Producer] \"%s\" ==> buffer\n", item);
        in = (in + 1) % BUFFER_SIZE;
        count++;
        DisplayBuffer();
        printf("Producer is leaving critical section.\n");
        printf("--------------------------------------------------------\n");

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    } while (repeat == 1);

    pthread_exit(0);
}

void *Consumer()
{
    do
    {
        char mesg[MAX_MESSAGE];

        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        printf("--------------------- CONSUMER -------------------------\n");
        printf("Consumer is entering critical section.\n");
        strcpy(mesg, buffer[out]);
        printf("[Consumer] buffer ==> \"%s\" ==> buffer\n", mesg);
        out = (out + 1) % BUFFER_SIZE;
        count--;
        DisplayBuffer();
        printf("Consumer is leaving critical section.\n");
        printf("--------------------------------------------------------\n");

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        printf("[Consumer] Consumed a message \"%s\" \n", mesg);
        sleep(rand() % 3 + 2);
    } while (repeat == 1);
    pthread_exit(0);
}

void DisplayBuffer()
{
    printf("Buffer contents:\n");
    printf("\tcount = %d, in = %d, out = %d\n", count, in, out);
    int p = out;
    for (int i = 0; i < count; i++)
    {
        printf("\t%d] %s\n", p, buffer[p]);
        p = (p + 1) % BUFFER_SIZE;
    }
}
