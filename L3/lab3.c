#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include  <sys/types.h>

struct shared_use_st {
	int signal;
	int number;
};

int main() {
	int running = 1;
	void *shared_memory = (void *)0;
	struct shared_use_st *shared_stuff;
	int shmid;

	srand((unsigned int)getpid());
	shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);

	if (shmid == -1) {
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	shared_memory = shmat(shmid, (void *)0, 0);

	if (shared_memory == (void *)-1) {
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	printf("Memory attached at %X\n", (int)shared_memory);

	shared_stuff = (struct shared_use_st *)shared_memory;
	shared_stuff->signal = 0;

	pid_t pid = fork();

	/**
	* Child will print generated numbers
	* if the number is over 50 it will stop and wait for
	* the parent to print in out as well then continue
	**/
	while(running) {
		if (pid != 0 && shared_stuff->signal == 1) {
			printf("Parent: %d\n", shared_stuff->number);
			shared_stuff->signal = 0;
		}

		if (pid == 0 && shared_stuff->signal == 0) {
			shared_stuff->number = rand()%100;

			if (shared_stuff->number > 50) {
				printf("Over 50: %d\n", shared_stuff->number);
				shared_stuff->signal = 1;
			}else{
				printf("Not over 5: %d\n", shared_stuff->number);
			}
			sleep(rand() % 2);
		}
	}

	if (shmdt(shared_memory) == -1) {
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}

	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}




