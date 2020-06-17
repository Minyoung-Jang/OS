#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define MAX_MESSAGE 64
char buffer[BUFFER_SIZE][MAX_MESSAGE];
int in = 0, out = 0, count = 0;

pthread_mutex_t mutex;
sem_t empty, full;
int empty_num = 0, full_num = 0;

int repeat = 1;

void DisplayBuffer(){
	printf("Buffer contents:\n");
	printf("\tcount = %d, in = %d, out = %d\n", count, in, out);
	int p = out;
	for(int i = 0; i < count; i++){
		printf("\t%d] %s\n", p, buffer[p]);
		p = (p + 1) % BUFFER_SIZE;
	}
}

void *Producer(void *param)
{
	int no_messages = 10;
	char *messages[64] = {
		"Nice to see you!",
		"Aal izz well!",
		"I love you! God loves you!",
		"God loves you and has a wonderful plan for your life.",
		"God bless you!",   
		"You are genius!",
		"Cheer up!",
		"Everything is gonna be okay.",
		"You are so precious!",
		"Hakuna matata!"
	};

	do{
		sleep(rand() % 3 + 1);  // Randomly wait for 1~3 seconds.

		char chosen_msg[MAX_MESSAGE];
		strcpy(chosen_msg, messages[rand() % no_messages]);
		printf("[Producer] Created a message \"%s\"\n", chosen_msg); // Generate a message

		// entry section
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);


		printf("---------------------PRODUCER---------------------\n");
		printf("Producer is entering critical section.\n");

		strcpy(buffer[in], chosen_msg);
		in = (in + 1) % BUFFER_SIZE;
		count++;
		printf("[Producer] \"%s\" ==> buffer\n", chosen_msg);

		DisplayBuffer();

		printf("Producer is leaving critical section.\n");
		printf("--------------------------------------------------\n");

		// exit section
		pthread_mutex_unlock(&mutex);
		sem_post(&full);

	}while(repeat);
}

void *Consumer(void *param)
{
	do{
		// entry section
		sem_wait(&full);
		pthread_mutex_lock(&mutex);


		printf("---------------------CONSUMER---------------------\n");
		printf("Consumer is entering critical section.\n");

		char out_msg[MAX_MESSAGE];
		strcpy(out_msg, buffer[out]);
		out = (out + 1) % BUFFER_SIZE;
		count--;

		printf("[Consumer] buffer ==> \"%s\"\n", out_msg);

		DisplayBuffer();

		printf("Consumer is leaving critical section.\n");
		printf("--------------------------------------------------\n");

		pthread_mutex_unlock(&mutex);
		sem_post(&empty);

		printf("[Consumer] Consumed a message \"%s\"\n", out_msg);
		sleep(rand() % 3 + 2);  // Randomly wait for 2~4 seconds.
	}while(repeat);
}

int main(int argc, char *argv[]){

	int duration = 0;
	int p = 0, c = 0;   // thread index

	if(argc == 2) duration = atoi(argv[1]);
	else duration = 30;

	srand(time(NULL));
	pthread_mutex_init(&mutex, NULL);
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);

	DisplayBuffer();

	pthread_t p_id, c_id;

	p = pthread_create(&p_id, NULL, Producer, NULL);
	if(p){
		printf("Producer is not created.\n");
		exit(1);
	}
	c = pthread_create(&c_id, NULL, Consumer, NULL);
	if(c){
		printf("Consumer is not created.\n");
		exit(1);
	}

	sleep(duration);

	repeat = 0;

	if (sem_getvalue(&full, &full_num) == 0)
    {
        printf("success getFull\n");
        printf("full_num:%d\n",full_num);
    }
    if(full_num==BUFFER_SIZE){
        sem_post(&full);
        printf("success postFull\n");
    }

    if (sem_getvalue(&empty, &empty_num) == 0)
    {
        printf("success getEmpty\n");
        printf("empty_num:%d\n",empty_num);
    }
    if(empty_num==BUFFER_SIZE){
        sem_post(&empty);
        printf("success postempty\n");
    }
	
	
	p = pthread_join(p_id, NULL);
	if(p){
		printf("Producer joining error.\n");
		exit(1);
	}
	c = pthread_join(c_id, NULL);
	if(c){
		printf("Consumer joining error.\n");
		exit(1);
	}
	printf("Bye!\n");

	pthread_mutex_destroy(&mutex);
	sem_destroy(&empty);
	sem_destroy(&full);

	return 0;
}
