// If the comments are hard to read because of the color,
// type ':set background=dark'

/***

	This program moves bars left and right until the user types 'q' key.
	Usage: $ ./bar <no_bar>

	Currently, it moves only one bar.
	Extend this program to move multiple bars using multiple threads.
	- Launch multiple threads, each of which moves a bar.
	- The vertical interval between bars should be height / (no_bar + 1).

	Compile with Console.c with -pthread -D_LINUX flag
		Ex) gcc bar.c Console.c -pthread -D_LINUX


***/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pthread.h>

#include "Console.h"

#define TRUE 1
#define FALSE 0

int repeat = TRUE;

typedef struct {
	int width, height;
	int x;
	int dx;
	char len;
	int step;
	float interval;
	int y;
	char c;
} ThreadParam;

void InitParam(ThreadParam *param, int width, int height, int no_bar, int ch);
void* Bar_Thread(void *vparam);
void DrawBar(int x, int y, int len, char c);


int main(int argc, char *argv[])
{
	int no_bar = 1;

	if(argc > 1)
		no_bar =atoi(argv[1]);
	else
		no_bar = 5;

	srand(time(NULL));
	int width = getWindowWidth();
	int height = getWindowHeight() - 1;
	
	clrscr();
	gotoxy(1, 1);
	printf("Press 'q' to quit.\n");

//  Do not enable the next line until you complete the program.
	EnableCursor(FALSE);

	ThreadParam param[no_bar];
	for(int i = 0; i < no_bar; i++){
		InitParam(&param[i], width, height, no_bar, i);
	}

	pthread_t bar_id[no_bar];
	pthread_attr_t bar_attr[no_bar];

	for(int i = 0; i < no_bar; i++){
		pthread_attr_init(&bar_attr[i]);
	}

	for(int i = 0; i < no_bar; i++){
		int t = pthread_create(&bar_id[i], &bar_attr[i], Bar_Thread, &param[i]);
		if(t){
			printf("Create Error.\n");
			exit(1);
		}
	}

	// TO DO: Wait for 'q' key. (use getch() and while statement)
	while(getch() != 'q')
		MySleep(1000);
	
	// TO DO: Stop threads by setting repeat to FALSE
	repeat = FALSE;

	// TO DO: Wait for the child threads

	for(int i = 0; i < no_bar; i++){
		int t = pthread_join(bar_id[i], NULL);
		if(t){
			printf("Join Error.\n");
			exit(1);
		}
	}

	gotoxy(1, 1);
	printf("Bye!\n");

//  Do not enable the next line until you complete the program.
	EnableCursor(TRUE);

	return 0;
}

void InitParam(ThreadParam *param, int width, int height, int no_bar, int ch){
	param->width = width;
	param->height = height;

	param->len = rand() % 6 + 5;	// a random number in [5, 10]
	param->step = rand() % 4 + 2;	// a random number in [2, 5]
	param->x = rand() % (param->width - param->step - 1) + 1; 
	param->c = 65+ch;

	// vertical interval between bars = height / (no_bar + 1)
	param->interval = param->height / (float)(no_bar + 1);
	param->y = (int)param->interval * (ch+1);
	param->dx = param->step;
	if(rand() % 2 == 0)
		param->dx = -param->dx;
}

void* Bar_Thread(void *vparam){
	ThreadParam *param = (ThreadParam*) vparam;
	while(repeat){
		int oldx = param->x;

		param->x += param->dx;
		if(param->x <= 0){
			param->dx = -param->dx;
			param->x = 1 - param->x;
		}

		if(param->x > param->width - param->len + 1){
			param->dx = -param->dx;
			param->x = param->width - param->len + 1 - (param->x - param->width + param->len - 1);
		}
	
		DrawBar(oldx, param->y, param->len, ' ');
		DrawBar(param->x, param->y, param->len, param->c);

		MySleep(50);
	}
}

void DrawBar(int x, int y, int len, char c)
{
	pthread_mutex_lock(&display_mutex);

	gotoxy(x, y);
	for(int i = 0; i < len; i++)
		putchar(c);
	fflush(stdout);

	pthread_mutex_unlock(&display_mutex);
}