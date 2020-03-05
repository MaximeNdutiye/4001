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

#define MAX_TEXT 512

int main(){
  int running = 1;
  int msgid;
  struct my_msg_st some_data;
  char buffer[BUFSIZ];

  // set up the message queueu
  // msgget(key_t, int) returns the System V message queue
  // identifier associated with the value of the key argument.
  msgid = msgget((key_t)1234, 0666 | IPC_CREAT);

  if (msgid == -1) {
    fprintf(stderr, "msgget failed with error: %d\n", errno);
    exit(EXIT_FAILURE);
  }

  // get text, add to message queue
  while(running) {
    printf("Enter some text: ");
    fgets(buffer, BUFSIZ, stdin);

    // using 1 as the message type
    some_data.my_msg_type = 1;
    strcpy(some_data.some_text, buffer);

    // send a message to the queue associated with the identifier msqid
    // msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
    // *msgp points to a struct defining long, char
    // msgsz is the size of message in bytes
    // msgflg action to take when some conditions are met
    if (msgsnd(msgid, (void *)&some_data, MAX_TEXT, 0) == -1) {
      fprintf(stderr, "msgsnd failed\n");
      exit(EXIT_FAILURE);
    }

    if (strncmp(buffer, "end", 3) == 0) {
      running = 0;
    }
  }

  if (msgctl(msgid, IPC_RMID, 0) == -1) {
    fprintf(stderr, "msgctl(IPC_RMID) failed\n");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}

// Some answers for part 6
//
// The mesage structure is
// typedef struct {
//  pid_t client_pid;
//  client_request_e request;
//  server_response_e response;
//  cdc_entry cdc_entry_data;
//  cdt_entry cdt_entry_data;
//  char error_text[ERR_TEXT_LEN + 1];
// } message_db_t;

// The server creates two message queues
//
// The client uses it's pid as the message type when sending messages
// to the server
//
// The client only receives messages with a type of it's pid

