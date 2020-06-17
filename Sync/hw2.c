#include <stdio.h>
#include <stdlib.h>       
#include <pthread.h>      
#include <semaphore.h>   
#include <unistd.h> 

#define NUMBER 3	   // thread�� ����
sem_t wrt,mutex;       // wrt��mutex�� semaphore����

int rand_value; //rand���� �ޱ����Ѻ���  
int readcount; //reader��ī��Ʈ��

int initialize();// �Լ� �ʱ�ȭ 
void * Writer(void *arg);// Writer �Լ�
void * Reader(void *arg);// Reader �Լ�


int main()
{
	pthread_t writer_thread, reader_thread[NUMBER];//writer��reader�� thread ����
	int i;
	int status=0;
	initialize();
    pthread_create(&writer_thread, NULL, &Writer, &status);//writer thread ����
	
	//////////////// reader��  thread ������//////////////////////////////////
	for(i=1; i<NUMBER; i++) //
	{
        pthread_create(&reader_thread[i], NULL, &Reader, (void*)&i);	  
		sleep(3); //3�ʰ� ���
	}
	/////////////////////////////////////////////////////////////////////////
	
	pthread_join(writer_thread, NULL);// writer������������ ���
    sem_destroy(&wrt);// �ʱ�ȭ�Ȱ��� write����
    sem_destroy(&mutex);// �ʱ�ȭ�Ȱ��� mutex����

	return 0;
}


////////////////�ʱ�ȭ�Լ�////////////////////
int initialize()
{
 sem_init(&wrt,0,1);//wrt �ʱ�ȭ
 sem_init(&mutex,0,1);//mutex �ʱ�ȭ
 return 0;
}
/////////////////////////////////////////////


/////////////////////////Writer �Լ�//////////////////////////////////////////////
void * Writer(void * arg)
{
	do
	{			
        srand((unsigned)time(NULL));//rand�ʱ�ȭ 
		sem_wait(&wrt);//reader���������� ���
		rand_value =(int)random()%100;//�����Ѱ�����
		printf("Writer THREAD0 put %d\n",rand_value);//���
        sleep(1);//1�ʴ��
		sem_post(&wrt);//�ٽý�����
		sleep(3);//3�ʴ��
	}while(1);

}
/////////////////////////////////////////////////////////////////////////////////


///////////////////////////Reader �Լ�/////////////////////////////////////////
void * Reader(void * arg)  
{
	int id;  //thread�� id
	id=*(int *)arg;
	readcount=0;//ī��Ʈ �ʱ�ȭ

    do
	{
		sem_wait(&mutex);//���(��ȣ����)
		readcount++;//ī��Ʈ����    
		if ( readcount==1 )//1�϶�
        sem_wait(&wrt);//����ɶ��������
		sem_post(&mutex);//�ٽý���

		printf("Reader THREAD%d first get %d\n", id, rand_value);//���
		sleep(1);//1�ʴ��
		printf("Reader THREAD%d second get %d\n", id, rand_value);//���

		sem_wait(&mutex);//����ɶ��������
		readcount--;//reader����Ϸ��� ī��Ʈ����
		if ( readcount==0 )//ī��Ʈ�� 0�϶�
		sem_post(&wrt);//writer�ٽý���
		sem_post(&mutex);//�ٽý���   
		sleep(3);//3�ʰ����
	}while(1);

}
