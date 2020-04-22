#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define THREADS 3

__thread int tls;
int global;

void *func(void *arg)
{
    int num = (int) arg;
    tls = num;
    global = num;

    sleep(1);

    printf("Thread  = %d, tls = %d, global = %d\n",num, tls, global);
}

int main()
{
    int ret;
    pthread_t thread[THREADS];
    int num;

    for(num = 0; num < THREADS; num++){
        ret = pthread_create(&thread[num], NULL, func, (void*)num);
    }

    for(num = 0; num < THREADS; num++){
        ret = pthread_join(thread[num], NULL);
    }

    return 0;
}