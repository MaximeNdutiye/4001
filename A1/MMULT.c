#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
* SYSC 4001 Assignment 1
* Maxime Ndutiye 101040467
*
*/

// 4x4 matrices M N and result Q
struct matrices {
  int M[4][4];
  int N[4][4];
  int Q[4][4];
};

// function declarations
static void printMatrix(int matrix[4][4]);
static void computeRow(struct matrices* matrices, int row);
static void seedMatrices(struct matrices* matrices);

int main() {
  void* mem_p = (void *)0;
  struct matrices* matrices_shm;
  int shmid;
  pid_t pid;

  // create shared memory
  shmid = shmget((key_t)12345, sizeof(struct matrices), 0666 | IPC_CREAT);
  if (shmid == -1) exit(EXIT_FAILURE);

  // attach shared memory
  mem_p = shmat(shmid, (void *)0, 0);
  if (mem_p == (void *)-1) exit(EXIT_FAILURE);

  // cast to something we can use
  matrices_shm = (struct matrices*)mem_p;

  // put data into matrices
  seedMatrices(matrices_shm);

  // create 4 child processes with fork
  // then compute a row of the result matrix Q
  for (int i = 0; i < 4; i++) {
    pid = fork();

    if (pid == 0) {
      computeRow(matrices_shm, i);
      exit(EXIT_SUCCESS);
    }
  }

  waitpid(-1, NULL, 0);

  // print the matrices
  printf("\nprinting M\n");
  printMatrix(matrices_shm->M);

  printf("\nprinting N\n");
  printMatrix(matrices_shm->M);

  printf("\nprinting Q\n");
  printMatrix(matrices_shm->Q);

  exit(EXIT_SUCCESS);
}

/**
* print out a matrix
*/
static void printMatrix(int matrix[4][4]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      printf("%i ", matrix[i][j]);
    }
    printf("\n");
  }
}

/**
* Compute a single row of Q
*/
static void computeRow(struct matrices* matrices, int row) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      matrices->Q[row][i] += matrices->M[row][j] * matrices->N[j][i];
    }
  }
}

/**
* Seed the matrices M and N with values
*/
static void seedMatrices(struct matrices* matrices) {
  int seed_M[4][4] = {10,20,30,40,5,6,7,8,4,3,2,1,8,7,6,5};
  int seed_N[4][4] = {10,30,50,70,2,4,6,8,7,2,5,7,8,6,2,4};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      matrices->M[i][j] = seed_M[i][j];
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      matrices->N[i][j] = seed_N[i][j];
    }
  }
}
