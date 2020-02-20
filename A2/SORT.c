#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>

/**
* SYSC 4001 Assignment 2
* Maxime Ndutiye 101040467
*
*/

// array of numbers to sort
struct numbers{
  int arr[5];
};

// function declarations
static int set_semvalue(void);
static void del_semvalue(void);
static int semaphore_p(void);
static int semaphore_v(void);
static void printArr(struct numbers* numbers_shm);
static void getValues(struct numbers* numbers_shm);
static void sort(int low, int hi, struct numbers* numbers_shm);
static int sorted(struct numbers* numbers_smh);

static int sem_id;

int main() {
  void* mem_p = (void *)0;
  struct numbers* numbers_shm;
  int shmid;
  pid_t pid;

  // create shared memory
  shmid = shmget((key_t)12345, sizeof(struct numbers), 0666 | IPC_CREAT);
  if (shmid == -1) exit(EXIT_FAILURE);

  // attach shared memory
  mem_p = shmat(shmid, (void *)0, 0);
  if (mem_p == (void *)-1) exit(EXIT_FAILURE);

  // cast to something we can use
  numbers_shm = (struct numbers*)mem_p;

  srand((unsigned int)getpid());
  sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);

  // create semaphore
  if (!set_semvalue()) {
    fprintf(stderr, "Failed to initialize semaphore\n");
    exit(EXIT_FAILURE);
  }

  // put data into matrices
  getValues(numbers_shm);

  // create 4 child processes with fork
  // then sort the values
  for (int i = 4; i > 0; i--) {
    pid = fork();

    // children sort
    if (pid == 0){
      sort(i-1, i, numbers_shm);
      exit(EXIT_SUCCESS);
    }
  }

  waitpid(-1, NULL, 0);

  // sorted array
  printf("\ndone sorting the array");
  printArr(numbers_shm);

  // parent delete semaphore
  if (pid != 0) {
    del_semvalue();
  }

  exit(EXIT_SUCCESS);
}

// sort a subarray of two elements in descending order
static void sort(int low, int hi, struct numbers* numbers_shm){
  printf("\nstarting to sort. low: %d, hi: %d \n", low, hi);

  while(!sorted(numbers_shm)) {
    if (!semaphore_p()) exit(EXIT_FAILURE); // get lock or block

    // swap
    if (numbers_shm->arr[low] < numbers_shm->arr[hi]) {
      int temp = numbers_shm->arr[low];
      numbers_shm->arr[low] = numbers_shm->arr[hi];
      numbers_shm->arr[hi] = temp;

      printf("\nswapped %d, %d\n", numbers_shm->arr[low], numbers_shm->arr[hi]);
    }

    if (!semaphore_v()) exit(EXIT_FAILURE); // release lock
  }

  exit(EXIT_SUCCESS);
}

// returns if an array is sorted
static int sorted(struct numbers* numbers_smh) {
  return numbers_smh->arr[0] >= numbers_smh->arr[1] &&
    numbers_smh->arr[1] >= numbers_smh->arr[2] &&
    numbers_smh->arr[2] >= numbers_smh->arr[3] &&
    numbers_smh->arr[3] >= numbers_smh->arr[4];
}

// create semaphore
static int set_semvalue(void){
  union semun sem_union;
  sem_union.val = 1;
  if (semctl(sem_id, 0, SETVAL, sem_union) == -1) return(0);
  return(1);
}

// delete semaphore
static void del_semvalue(void){
  union semun sem_union;
  if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
  fprintf(stderr, "Failed to delete semaphore\n");
}

// acquire a lock or block
static int semaphore_p(void){
  struct sembuf sem_b;
  sem_b.sem_num = 0;
  sem_b.sem_op = -1; /* P() */
  sem_b.sem_flg = SEM_UNDO;

  if (semop(sem_id, &sem_b, 1) == -1) {
    fprintf(stderr, "semaphore_p failed\n");
    return(0);
  }
  return(1);
}

// release a lock
static int semaphore_v(void){
  struct sembuf sem_b;
  sem_b.sem_num = 0;
  sem_b.sem_op = 1; /* V() */
  sem_b.sem_flg = SEM_UNDO;

  if (semop(sem_id, &sem_b, 1) == -1) {
    fprintf(stderr, "semaphore_v failed\n");
    return(0);
  }

  return(1);
}

// print the array
static void printArr(struct numbers* numbers_shm){
  printf("\n");

  for(int i=0; i<5; i++){
    printf("%d ", numbers_shm->arr[i]);
  }

  printf("\n");
}

// get values from the user
static void getValues(struct numbers* numbers_shm){
  int i=0;
  int intInput;

  while(i<5){
    printf("Enter an integer: ");
    scanf("%d", &intInput);

    numbers_shm->arr[i] = intInput;
    i++;

  }

  printArr(numbers_shm);
}

