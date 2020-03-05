#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>

struct my_msg_st {
  long int my_msg_type;
  char some_text[BUFSIZ];
};

int main(){
  int running = 1;
  int msgid;
  struct my_msg_st some_data;
  long int msg_to_receive = 0;

  // set up the message queueu
  msgid = msgget((key_t)1234, 0666 | IPC_CREAT);

  if (msgid == -1) {
    fprintf(stderr, "msgget failed with error: %d\n", errno);
    exit(EXIT_FAILURE);
  }

  // retreive messages untill we receive end
  while(running) {
    // msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp,int msgflg);
    // receive message of length msgzs from queue with msqid, place it into msgp
    // msgtyp: receive the first message if 0, else first message of type msgtyp
    // msgflg: action to take if message of msgtyp is not in queue
    if (msgrcv(msgid, (void *)&some_data, BUFSIZ,msg_to_receive, 0) == -1) {
      fprintf(stderr, "msgrcv failed with error: %d\n", errno);
      exit(EXIT_FAILURE);
    }

    printf("You wrote: %s", some_data.some_text);

    if (strncmp(some_data.some_text, "end", 3) == 0) {
      running = 0;
    }
  }

  if (msgctl(msgid, IPC_RMID, 0) == -1) {
    fprintf(stderr, "msgctl(IPC_RMID) failed\n");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
