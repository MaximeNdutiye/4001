#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MICRO_SEC_IN_SEC 1000000

int main(){
	pid_t pid;
	char *message;
	int n, i;
	struct timeval start, end;

	printf("fork program starting\n");

	gettimeofday(&start, NULL);

	pid = fork();

	gettimeofday(&end, NULL);

	// test computation time of fork(), average delta 200 - 500 nanoseconds
	printf("\nElapsed Time: %ld micro sec\n", ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec)
				- (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec)));

	// for my run the parent had pid 34616, child 34617 with ppid 34616
	// they share the same properties accept for pid, ppid, S

	// fork returns the child's pid to the parent process
	// fork return 0 to the child process
	switch(pid){
		case -1:
			perror("fork failed");
			exit(1);
		case 0:
			message = "This is the child";
			n = 5;
			printf("child process, fork value: %d\n", getpid());

			// we can still fork in a child process
			// pid_t new_pid = fork();
		break;
		default:
			message = "This is the parent";
			printf("in parent we obtain child pid as: %d\n",pid);
			n = 3;
		break;
	}

	for(; n > 0; n--) {
		puts(message);
		sleep(1);
	}

	exit(0);
}
