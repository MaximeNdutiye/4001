#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>

void *thread_function(void *arg);

int main(){
  struct timeval start, end;
  pthread_t a_thread;
  pid_t pid;
  int res;

  char message[] = "Hello World";

  gettimeofday(&start, NULL);

  // add fork() to create a process
  pid = fork();
  gettimeofday(&end, NULL);

  printf("%ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

  gettimeofday(&start, NULL);

  // add pthread_create() to create a thread
  res = pthread_create(&a_thread, NULL, thread_function, (void *)message);
  gettimeofday(&end, NULL);

  printf("%ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

 return 0;
}

void *thread_function(void *arg){
  return NULL;
}
