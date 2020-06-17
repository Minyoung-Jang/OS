////////////////헤더파일 선언///////////////////////
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
//////////////////////////////////////////////////////

/////////////////////////////////////////////////////
#define THREADNUM 3 //process의 수
#define MAXITEM 100 //최대아이템수
/////////////////////////////////////////////////////

//////////////////구조체 선언///////////////////////
struct data
{
	int buff[MAXITEM];//버퍼
	int cnt_item;//버퍼에 저장한 아이템의 총개수
	int cnt_slot;//버퍼에서 읽어들인 아이템수
}Data;
//////////////////////////////////////////////////////

pthread_mutex_t mutex;//mutex선언

void *Producer(void *data);//Producer선언
void *Consumer(void *data);//Consumer선언

int main()
{
	pthread_t producer[THREADNUM],consumer;// producer와consumer의 thread선언
    
	////변수선언/////////////
	int i=0;//
	int a =1;
	int count[THREADNUM];
	//////////////////////////
    
	//////////////초기화///////
	Data.cnt_item = 0;		
	Data.cnt_slot =0;
	///////////////////////////
    
	///////////////////producer thread선언///////////////////////////
	for(i=0;i<THREADNUM;i++)
    {
		pthread_create(&producer[i],NULL,Producer,(void *)&i);
	}
	////////////////////////////////////////////////////////////////

	/////////////////////////consumer thread선언///////////////////
	pthread_create(&consumer,NULL,Consumer,(void *)&a);
	
	///////producer와 consumer join///////////////////////////
	for(i=0;i<THREADNUM;i++)// 끝날때까지 main대기
	{
	  pthread_join(producer[i],NULL);//producer 종료대기
	  pthread_join(consumer,NULL);//consumer 종료대기
	}
	////////////////////////////////////////////////////////
	return 0;
}

void *Producer(void *data)//Producer함수
{
	/////변수선언//////
	int threadNum;
	int i;
    ///////////////////

	threadNum=*(int *)data;//쓰레드아이디

    while(Data.cnt_item<MAXITEM)//최대크기 전까지
	{
		if(Data.cnt_slot < 0)   //overflow방지
		{
			pthread_mutex_unlock(&mutex);//unlock
			sleep(1);//1초대기
		}
		pthread_mutex_lock(&mutex);//mutex lock요청
        Data.buff[Data.cnt_item]=Data.cnt_item*Data.cnt_item;//버퍼의데이터
        printf("Producer THREAD%d put %d on the buffer[%d]      cnt_item %d\n",//출력
		threadNum,Data.buff [Data.cnt_item],Data.cnt_item,Data.cnt_item+1);
		Data.cnt_item++;//저장했으므로 증가
		pthread_mutex_unlock(&mutex);//unlock 요청
        sleep(2);//2초대기
	}	
}
void *Consumer(void *data)//consumer함수 
{
	while(Data.cnt_slot<MAXITEM)//최대크기전까지
	{
      pthread_mutex_lock(&mutex);//unlock요청
      Data.buff[Data.cnt_slot]=Data.cnt_slot*Data.cnt_slot;//버퍼의데이터
      printf("Consumer THREAD3 put %d on the buffer[%d]      cnt_item %d\n", Data.buff [Data.cnt_slot],Data.cnt_slot,Data.cnt_slot+1);//출력
      Data.buff[Data.cnt_slot]=0;//소비하였으므로 초기화		
      Data.cnt_slot++;//읽은수증가
	  pthread_mutex_unlock(&mutex);//unlock
      sleep(1);//1초대기
	}
}

