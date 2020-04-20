#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
void *thread_function(void *arg);

int main()
{
  int res;
  pthread_t a_thread;
  void *thread_result;
  res = pthread_create(&a_thread, NULL, thread_function, NULL);

  if (res != 0)
  {
    perror("Thread creation failed");
    exit(EXIT_FAILURE);
  }

  sleep(3);
  printf("Canceling thread...\n");
  res = pthread_cancel(a_thread);

  if (res != 0)
  {
    perror("Thread cancelation failed");
    exit(EXIT_FAILURE);
  }

  printf("Waiting for thread to finish...\n");
  res = pthread_join(a_thread, &thread_result);

  if (res != 0)
  {
    perror("Thread join failed");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}

void *thread_function(void *arg){
  while(0){
    printf("yoyo\n");
  }

  return NULL;
}