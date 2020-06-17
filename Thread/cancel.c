#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct
{
	int dim;
	float *data;
} Vector;

void CreateVector(Vector *vec, int dim);
void DeleteVector(Vector *vec);
void DisplayVector(Vector *vec, char *mesg);
void FillVectorIncreasing(Vector *vec);
void FillVectorDecreasing(Vector *vec);
void clean_up(void *arg);
void VectorAdd_MT(Vector *vec1, Vector *vec2, Vector *res, int no_thread);

__thread int tls;

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Usage: %s <dim> <# of threads>\n", argv[0]);
		return -1;
	}
	int dim = atoi(argv[1]);
	int no_thread = atoi(argv[2]);
	printf("Addtipying %d-dim  vectors using %d threads\n", dim, no_thread);

	Vector vecA = {0};
	Vector vecB = {0};
	Vector sum = {0};

	CreateVector(&vecA, dim);
	FillVectorIncreasing(&vecA);
	if (dim < 20)
		DisplayVector(&vecA, "A = ");

	CreateVector(&vecB, dim);
	FillVectorDecreasing(&vecB);
	if (dim < 20)
		DisplayVector(&vecB, "B = ");

	CreateVector(&sum, dim);

	struct timeval start_time, end_time;
	gettimeofday(&start_time, NULL);

	VectorAdd_MT(&vecA, &vecB, &sum, no_thread);

	gettimeofday(&end_time, NULL);
	end_time.tv_sec -= start_time.tv_sec;
	end_time.tv_usec -= start_time.tv_usec;
	if (end_time.tv_usec < 0)
	{ // handle carry
		end_time.tv_sec--;
		end_time.tv_usec += 1000000;
	}

	if (dim < 20)
		DisplayVector(&sum, "A + B = ");
	printf("computing time = %ld sec %ld microsec\n", end_time.tv_sec, end_time.tv_usec);

	DeleteVector(&vecA);
	DeleteVector(&vecB);
	DeleteVector(&sum);

	return 0;
}

void CreateVector(Vector *vec, int dim)
{
	if (vec->data)
		free(vec->data);

	vec->data = malloc(dim * sizeof(float));
	if (vec->data == NULL)
	{
		printf("Failed to allocate memory in %s.\n", __FUNCTION__);
		vec->dim = 0;
		return;
	}

	memset(vec->data, 0, dim * sizeof(float));

	vec->dim = dim;
}

void DeleteVector(Vector *vec)
{
	if (vec)
	{
		free(vec->data);
		vec->dim = 0;
	}
}

void DisplayVector(Vector *vec, char *mesg)
{
	if (mesg != NULL)
		printf("%s\n", mesg);

	printf("%d-dim vector:\n", vec->dim);

	for (int i = 0; i < vec->dim; i++)
		printf("%5.1f ", vec->data[i]);

	printf("\n");
}

void FillVectorIncreasing(Vector *vec)
{
	for (int i = 0; i < vec->dim; i++)
		vec->data[i] = i + 1;
}

void FillVectorDecreasing(Vector *vec)
{
	for (int i = 0; i < vec->dim; i++)
		vec->data[i] = vec->dim - i;
}

typedef struct
{
	Vector *vec1;
	Vector *vec2;
	Vector *res;

	int thread_idx;
	int no_thread;
} VectorAdd_param;

void *VectorAdd_Thread(void *vparam);

void clean_up(void *arg)
{
	printf("Thread cancel Clean_up function\n");
}

void VectorAdd_MT(Vector *vec1, Vector *vec2, Vector *res, int no_thread)
{
	int t = 0;
	int scope;

	pthread_t tid[no_thread];
	pthread_attr_t attr[no_thread];

	VectorAdd_param param[no_thread];

	for (t = 0; t < no_thread; t++)
	{
		param[t].vec1 = vec1;
		param[t].vec2 = vec2;
		param[t].res = res;
		param[t].thread_idx = t;
		param[t].no_thread = no_thread;
	}

	// for (t = 0; t < no_thread; t++)
	// {
	// 	pthread_attr_init(&attr[t]);
		//pthread_attr_setscope(&attr[t], PTHREAD_SCOPE_PROCESS);
		//pthread_attr_setscope(&attr[t],PTHREAD_SCOPE_SYSTEM);
		//pthread_attr_setschedpolicy(&attr[t], SCHED_FIFO);
		//pthread_attr_setschedpolicy(&attr[t],SCHED_RR);

		// pthread_attr_getscope(&attr[t], &scope);
		// if (scope == PTHREAD_SCOPE_SYSTEM)
		// {
		// 	printf("user mode thread\n");
		// }
		// else if (scope == PTHREAD_SCOPE_PROCESS)
		// {
		// 	printf("Kernel mode thread\n");
		// }
	// }

	for (t = 0; t < no_thread; t++)
	{
		pthread_create(&tid[t], NULL, VectorAdd_Thread, &param[t]);
		//sleep(1);
		pthread_cancel(tid[t]);
	}
	//print("hi\n");
	

	for (t = 0; t < no_thread; t++)
		pthread_join(tid[t], NULL);
}
void *VectorAdd_Thread(void *vparam)
{

	VectorAdd_param *param = (VectorAdd_param *)vparam;
	Vector *vec1 = param->vec1;
	Vector *vec2 = param->vec2;
	Vector *res = param->res;

	//종료 요청 무시

	// pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

	//종료 요청 적용

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);

	//pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	//sleep(1);
	printf("hello\n");
	//잘 짜먄 여기 앞에서 끝남 
	pthread_testcancel();
	

	

	for (int i = param->thread_idx; i < vec1->dim; i += param->no_thread)
	{
		res->data[i] = vec1->data[i] + vec2->data[i];

		//thread 마다 tls가 생김
		tls=i;
	}
	//Deferred에서 끝내도 되는 분기점
	

	printf("%d\n",tls);
	//pthread_testcancel();
}