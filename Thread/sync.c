#include <signal.h>
#include <unistd.h>
#include <stdio.h>

//user level의 interrupt

void sig_handler(int signo);
void sig_handler2(int signo);

int main(int argc, char **argv)
{
  int i = 0;
  //9번 signal 제외하곤 바꿔치기 가능
  signal(SIGINT, (void *)sig_handler);
  //signal(SIGQUIT, (void *)sig_handler2);
  while(1)
  {
    printf("%d\n", i);
    sleep(2);
    i++;
  }
  return 1;
}

//override 
void sig_handler(int signo)
{
  printf("I Received SIGINT(%d)\n", SIGINT);
  sleep(4);
}

void sig_handler2(int signo)
{
  printf("I Received SIGQUIT(%d)\n", SIGQUIT);
  sleep(4);
}