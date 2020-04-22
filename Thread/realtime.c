#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

#define NUM_THREADS 5

void *runner(void *arg)
{
    printf("RUN\n");
}


int main()
{
    int i = 0, policy = 0;
    pthread_t tid[NUM_THREADS];
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    if(pthread_attr_getschedpolicy(&attr, &policy) != 0){
        fprintf(stderr, "Unable to get policy\n");
    }else{
        if(policy == SCHED_OTHER){
            printf("OTHER\n");
        }else if(policy == SCHED_RR){
            printf("RR\n");
        }else if(policy == SCHED_FIFO){
            printf("FIFO\n");
        }
    }

    if(pthread_attr_setschedpolicy(&attr, SCHED_RR) != 0){
        fprintf(stderr, "Unable to set policy\n");
    }else{
        pthread_attr_getschedpolicy(&attr, &policy);
        if(policy == SCHED_OTHER){
            printf("Changed OTHER\n");
        }else if(policy == SCHED_RR){
            printf("Changed RR\n");
        }else if(policy == SCHED_FIFO){
            printf("Changed FIFO\n");
        }
    }

    for(i = 0; i < NUM_THREADS; i++){
        pthread_create(&tid[i], &attr, runner, NULL);
    }

    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(tid[i], NULL);
    }

    return 0;
}