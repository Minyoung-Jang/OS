char notice[] = "If the following comments are too dark to read, type ':set background=dark'.";

/*****

	ECE30021 Operating Systems (2020 Spring) Final Test

	아래 프로그램은 Readers-Writers Problem에 대한 해법이다.
	Linux 환경에서 vi, gcc, pthread를 사용하여 완성하시오.
	Reader와 Writer의 동작은 각각 다음과 같다.
	
	* Behavior of each writer
		- If a thread is in the critical section, all writers must wait.
		- The writer can enter the critical section only when no thread is
in its critical section.
		- It should prevent all threads from entering the critical section.

	* Behavior of each reader
		- If no writer is in its critical section, the reader can enter the
critical section.
		- Otherwise, the reader should wait until the writer leaves the
critical section.
		- When a reader is in its critical section, any reader can enter the
critical section, but no writer can.
		- Condition for the first reader is different from the following readers.

	데모 실행파일  reader_writer 실행 방법:

		chmod u+x reader_writer		// set execution permission
		./reader_writer 10 3 3		// run for 10 sec with 3 readers and 3 writers


	main()은 복수의 reader threads와 writer threads를 생성한다.
	Reader()와 Writer()는 각각 reader, writer thread를 위한 thread function이다.


	Problem 1: pthread_rwlock_t를 이용해서 아래 프로그램을 완성하시오.
		- 다음 API 함수들을 이용하시오.
		int pthread_mutex_init(pthread_mutex_t * mutex, const pthread_mutex_attr *attr);
		int pthread_mutex_lock(pthread_mutex_t *mutex);
		int pthread_mutex_unlock(pthread_mutex_t *mutex);
		int pthread_mutex_destory(pthread_mutex_t *mutex);

		int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
		int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
		int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
		int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
		int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);

		- 완성된 코드는  reader_writer_1.c 라는 이름의 파일로 제출하시오.

	Problem 2: pthread_rwlock_t 및 관련 함수들을 이용하지 말고 pthread_mutex_t만 이용하여 완성하시오.
		- 교재에 소개된 The Readers-Writers Problem 해법 알고리즘을 참조해 int 변수와 mutex를 이용해 구현하시오.
		- 완성된 코드는  reader_writer_2.c 라는 이름의 파일로 제출하시오.


	필요할 경우 structure에field를 추가하는 것은 허용되나 global variable은 추가하지 마시오.
	두 문제의 답안은 HISNet 과제보드에 각각 제출하시오.
	두 파일은 모두 reader_writer처럼 동작해야 함.

*****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#define TRUE 1
#define FALSE 0


typedef struct {
	char *message;					// the message of board (dynamic memory block)
	int max_len;					// maximum length of the message (specified by InitNoticeBoard())
	
	int reader_in_cs;				// # of reader threads in the critical section
	pthread_mutex_t mutex_rcs;		// mutex to prevent race condition on reader_in_cs

	int writer_in_cs;				// # of writer threads in the critical section
	pthread_mutex_t mutex_wcs;		// mutex to prevent race condition on writer_in_cs


	// TO DO: declare fields and objects or synchronization
	sem_t write, mutex;
	int readcount;

} NoticeBoard;

void InitNoticeBoard(NoticeBoard *board, int max_len);		// initialize NoticeBoard object
void DestroyNoticeBoard(NoticeBoard *board);				// deallocate NoticeBoard object

void WriteMessage(NoticeBoard *board, const char *mesg, int thread_id);		 // write a message on the board
void ReadMessage(NoticeBoard *board, char *mesg, int max_len, int thread_id);// read the message on the board

void ReleaseThreads(NoticeBoard *board, int count);			// release all waiting threads
int CheckNoticeBoard(NoticeBoard *board);					// check violation

void AddReaderInCS(NoticeBoard *board, int inc);			// atomically increase or decrease reader_in_cs
void AddWriterInCS(NoticeBoard *board, int inc);			// atomically increase or decrease writer_in_cs

void msleep(int msec);

typedef struct {
	int type;		// 1: reader thread, 2: writer thread
	int id;			// thread id

	NoticeBoard *board;
} ThreadParam;

int repeat = TRUE;					// for loops in the threads

void* Reader(void *vparam);			// thread function of the readers
void* Writer(void *vparam);			// thread function of the writers

int main(int argc, char *argv[])
{
	srand(time(NULL));

	if(argc == 1){
		printf("Usage: %s <duration(sec)> [no_reader] [no_writer]\n", argv[0]);
		return 0;
	}

	int duration = 0;
	int no_reader = 0;
	int no_writer = 0;

	if(argc >= 2)
		duration = atoi(argv[1]);
	if(duration == 0)
		duration = 10;		// default

	if(argc >= 3)
		no_reader = atoi(argv[2]);
	if(no_reader == 0)
		no_reader = 3;		// default

	if(argc >= 4)
		no_writer = atoi(argv[3]);
	if(no_writer == 0)
		no_writer = 3;		// default

	printf("duration = %d, no_reader = %d, no_writer = %d\n", duration, no_reader, no_writer);
	
	NoticeBoard board = { 0 };
	InitNoticeBoard(&board, 256);
	

	// TO DO: launch writer threads using Writer()
	pthread_t r_id[no_reader], w_id[no_writer];
	pthread_attr_t reader_attr[no_reader], writer_attr[no_writer];
	int r = 0, w = 0;
	for(int i = 0; i < no_writer; i++){
		pthread_attr_init(&writer_attr[no_writer]);
	}

	for(int i = 0; i < no_writer; i++){
		w = pthread_create(&w_id[i], &writer_attr[i], &Writer, &board);
		if(w){
			printf("Writer Error");
			exit(0);
		}
	}

	// TO DO: launch reader threads using Reader()
	for(int i = 0; i < no_reader; i++){
		pthread_attr_init(&reader_attr[i]);
	}

	for(int i = 0; i < no_reader; i++){
		r = pthread_create(&r_id[i], &reader_attr[i], &Reader, &board);
		if(r){
			printf("Reader Error");
			exit(0);
		}
	}

	sleep(duration);

	printf("================== Terminating Threads =====================\n");
	fflush(stdout);

	repeat = FALSE;

	printf("Waiting for 5 sec...\n");
	fflush(stdout);

	sleep(5);			// wait for 5 sec.

	printf("Releasing threads that might be waiting...\n");
	fflush(stdout);
	ReleaseThreads(&board, no_reader + no_writer);		// release all threads waiting for any mutex or rwlock

	printf("Waiting for threads...\n");
	fflush(stdout);

	// TO DO: wait for the readers and writers
	for(int i = 0; i < no_reader; i++){
		pthread_join(r_id[i], NULL);
	}
	for(int i = 0; i < no_writer; i++){
		pthread_join(w_id[i], NULL);
	}


	printf("Destroying NoticeBoard...\n");
	fflush(stdout);
	DestroyNoticeBoard(&board);

	printf("Bye!\n");									// This message MUST be displayed.

	return 0;
}

void* Writer(void *vparam)
// DO NOT modify this function
{
	ThreadParam *param = (ThreadParam*)vparam;

	int no_messages = 10;
	char *messages[64] =  {
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


	char mesg[256];
	int t = 0;

	while(repeat){
		// produce a message
		strcpy(mesg, messages[rand() % no_messages]);
		printf("[Writer %d, t = %d] Produced a new message \"%s\".\n", param->id, t, mesg);
		fflush(stdout);

		// write the message on the board
		WriteMessage(param->board, mesg, param->id);

		// wait for 1000 ~ 4000 msec. 
		msleep(rand() % 3000 + 1000);

		t++;
	}
}

void* Reader(void *vparam)
// DO NOT modify this function
{
	ThreadParam *param = (ThreadParam*)vparam;

	char mesg[256];
	int t = 0;

	while(repeat){
		// read a message from the board
		ReadMessage(param->board, mesg, 256, param->id);

		// consume the message
		printf("[Reader %d, t = %d] Consumed a message \"%s\"\n", param->id, t, mesg);
		fflush(stdout);

		// wait for 1000 ~ 4000 sec. 
		msleep(rand() % 3000 + 1000);
		t++;
	}
}

void InitNoticeBoard(NoticeBoard *board, int max_len)
// TO DO: implement this function.
{
	// TO DO: dynamically allocate board->message and set variables
	board->message = (char*)malloc(sizeof(char)*max_len);
	board->max_len = max_len;
	board->message[0] = 0;

	board->reader_in_cs = 0;
	board->writer_in_cs = 0;

	// TO DO: initialize mutex_rcs and mutex_wcs
	pthread_mutex_init(&board->mutex_rcs, NULL);
	pthread_mutex_init(&board->mutex_wcs, NULL);

	// TO DO: initialize or create fields and objects or synchronization
	sem_init(&board->write, 0, 1);
	sem_init(&board->mutex, 0, 1);
	board->readcount = 0;

}

void DestroyNoticeBoard(NoticeBoard *board)
// TO DO: implement this function.
{
	// TO DO: deallocate all memory blocks and synchronization objects
	pthread_mutex_destroy(&board->mutex_rcs);
	pthread_mutex_destroy(&board->mutex_wcs);
	sem_destroy(&board->write);
	sem_destroy(&board->mutex);
	free(board->message);
}

void WriteMessage(NoticeBoard *board, const char *mesg, int thread_id)
// TO DO: complete this function by implementing the entry and exit sections
{
	sem_wait(&board->write);
	
	AddWriterInCS(board, 1);

	printf("[Writer %d] Writing message \"%s\". (writer:reader = %d:%d)\n", thread_id, mesg, board->writer_in_cs, board->reader_in_cs);
	fflush(stdout);
	strcpy(board->message, mesg);

	CheckNoticeBoard(board);					// check violation

	msleep(rand() % 2000 + 1000);				// wait for 1000 ~ 3000 msec.

	AddWriterInCS(board, -1);

	sem_post(&board->write);
}

void ReadMessage(NoticeBoard *board, char *mesg, int max_len, int thread_id)
// TO DO: complete this function by implementing the entry and exit sections
{
	sem_wait(&board->mutex);
	AddReaderInCS(board, 1);
	if(board->readcount == 1) sem_wait(&board->write);
	sem_post(&board->mutex);

	strncpy(mesg, board->message, max_len);
	mesg[max_len - 1] = 0;					// for safety
	printf("[Reader %d] Read message \"%s\". (writer:reader = %d:%d)\n", thread_id, mesg, board->writer_in_cs, board->reader_in_cs);
	fflush(stdout);

	CheckNoticeBoard(board);				// check violation

	msleep(rand() % 3000 + 1000);			// wait for 1~4 sec.

	sem_wait(&board->mutex);
	AddReaderInCS(board, -1);
	if(board->readcount == 0) sem_post(&board->write);
	sem_post(&board->mutex);
}

void ReleaseThreads(NoticeBoard *board, int count)
// TO DO: implement this function.
{
	// TO DO: unlock all mutex (and rwlocks) of board count times
	//        to release the threads that might be waiting for a mutex (or an rwlock)
	pthread_mutex_unlock(&board->mutex_rcs);
	pthread_mutex_unlock(&board->mutex_wcs);
	sem_post(&board->mutex);
	sem_post(&board->write);
}

void AddReaderInCS(NoticeBoard *board, int inc)
// DO NOT modify this function
{
	if(repeat)			// to avoid unnecsssary waiting
		pthread_mutex_lock(&board->mutex_rcs);

	board->reader_in_cs += inc;

	pthread_mutex_unlock(&board->mutex_rcs);
}

void AddWriterInCS(NoticeBoard *board, int inc)
// DO NOT modify this function
{
	if(repeat)			// to avoid unnecsssary waiting
		pthread_mutex_lock(&board->mutex_wcs);

	board->writer_in_cs += inc;

	pthread_mutex_unlock(&board->mutex_wcs);
}

int CheckNoticeBoard(NoticeBoard *board)
// DO NOT modify this function
{
	if(repeat == FALSE)		// bypass violation check if the threads are terminating.
		return TRUE;

	if(board->writer_in_cs > 0){
		if(board->writer_in_cs > 1 || board->reader_in_cs > 0){
			printf("Error! writer_in_cs = %d, reader_in_cs = %d\n", board->writer_in_cs, board->reader_in_cs);
			exit(-1);
		}
	}

	return TRUE;
}

void msleep(int msec)
{
	if(repeat)
		usleep(msec * 1000);
}
