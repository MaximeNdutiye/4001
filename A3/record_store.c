#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <errno.h>

// declarations
struct employee{
  char name[12];
  char department_name[12];
  char employee_number[12];
  char salary[12];
};

struct msg_st {
  long int type;
  char text[BUFSIZ];
};

static void admin(int client_msgid, int admin_msgid);
static void client(int client_msgid, int admin_msgid);

int main(int argc, char *argv[]){
  int admin_msgid;
  int client_msgid;

  struct msg_st data;
  struct employee employee_data;

  long int msg_to_receive = 0;

  // set up the message queueus
  admin_msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
  client_msgid = msgget((key_t)4321, 0666 | IPC_CREAT);

  if (admin_msgid == -1 || client_msgid == -1) {
    fprintf(stderr, "msgget failed with error: %d\n", errno);
    exit(EXIT_FAILURE);
  }

  // no args passed then we are client
  if (argc > 1){
    client(client_msgid, admin_msgid);
  }else{
    admin(client_msgid, admin_msgid);
  }

  return 0;
}

static void admin(int client_msgid, int admin_msgid){
  char buffer[BUFSIZ];
  int store_size = 0;
  int max_store_size = 100;
  int found = 0;
  int running = 1;

  // user args that can  be passed
  char employee_number[12];
  char employee_name[12];
  char department_name[12];

  // record store
  struct msg_st msg_data;
  struct employee record_store[max_store_size];
  struct employee new_employee;

  while(running){
    found = 0;

    printf("Record store waiting...\n\n");

    if (msgrcv(admin_msgid, (void *)&msg_data, BUFSIZ, 0, 0) == -1) {
      fprintf(stderr, "msgrcv failed with error: %d\n", errno);
      exit(EXIT_FAILURE);
    }

    // let's end
    if (strncmp(msg_data.text, "end", 3) == 0) {
      printf("ending");
      running = 0;
      return;
    }

    // split the string and get the data from it
    char *ptr = strtok(msg_data.text, " ");

    if (strcmp(ptr, "insert") == 0) {
      printf("insert command\n");

      // we should have 4 args next
      ptr = strtok(NULL, " ");
      strcpy(new_employee.name, ptr);

      ptr = strtok(NULL, " ");
      strcpy(new_employee.department_name, ptr);

      ptr = strtok(NULL, " ");
      strcpy(new_employee.employee_number, ptr);

      ptr = strtok(NULL, " ");
      strcpy(new_employee.salary, ptr);

      // save employee into the record store
      if (store_size < max_store_size){
        record_store[store_size] = new_employee;
        store_size++;
        strcpy(buffer, "success");
      }else{
        strcpy(buffer, "fail");
      }

    }else if(strcmp(ptr, "check_employee_number") == 0){
      ptr = strtok(NULL, " ");
      strcpy(employee_name, ptr);

      int len = strlen(employee_name);

      printf("check employee number: %s", employee_name);

      for (int i=0; i<store_size; i++){
        if (strcmp(record_store[i].name, employee_name) == 0){
          strcpy(buffer, record_store[i].employee_number);
          found = 1;
          break;
        }
      }

      if (found == 0){
        strcpy(buffer, "not found");
      }

    // department name
    }else if(strcmp(ptr, "check") == 0){
      ptr = strtok(NULL, " ");
      strcpy(department_name, ptr);

      printf("check department name: %s \n", department_name);

      strcpy(buffer, "employee numbers: ");

      for (int i=0; i<store_size; i++){
        if (strcmp(record_store[i].department_name, department_name) == 0){
          strcat(buffer, record_store[i].employee_number);
          strcat(buffer, " ");

          found++;
        }

        // cap the number of returned employees to 5
        if (found == 5){
          break;
        }
      }

    }else if(strcmp(ptr, "check_salary") == 0){
      ptr = strtok(NULL, " ");
      strcpy(employee_number, ptr);

      printf("check salary: %s\n", employee_number);

      for (int i=0; i<store_size; i++){
        if (strcmp(record_store[i].employee_number, employee_number) == 0){
          strcpy(buffer, record_store[i].salary);
          found = 1;
          break;
        }
      }

      if (found == 0){
        strcpy(buffer, "not found");
      }
    }else if(strcmp(ptr, "delete") == 0){
      int position = 0;

      ptr = strtok(NULL, " ");
      strcpy(employee_number, ptr);

      printf("delete command: %s\n", employee_number);

      // find position of employee
      for (int i=0; i<store_size; i++){
        if (strcmp(record_store[i].employee_number, employee_number) == 0){
          found = 1; position = i;
          strcpy(buffer, "0");

          break;
        }
      }

      if (found){
        for(int i=0; i<=store_size; i++){
          record_store[i] = record_store[i+1];
        }

        store_size--;
      }else{
        strcpy(buffer, "-1");
      }

    // bad command
    }else{
      strcpy(buffer, "invalid command");
      printf("bad command\n");
    }

    // server response will be stored in buffer
    msg_data.type = 1;
    strncpy(msg_data.text, buffer, BUFSIZ);

    // send data
    if (msgsnd(client_msgid, (void *)&msg_data, BUFSIZ, 0) == -1) {
      fprintf(stderr, "msgsnd failed\n");
      exit(EXIT_FAILURE);
    }
  }
}

static void client(int client_msgid, int admin_msgid){
  char buffer[BUFSIZ];
  struct msg_st msg_data;
  int running = 1;

  printf("Enter a command\n");
  printf("commands start with the command name followed by space separated arguemnts\n");
  printf("max length for all fields is 12 characters\n\n");

  printf("insert name department_name employee_number salary\n");
  printf("check_employee_number name\n");
  printf("check_salary employee_number\n");
  printf("check department_name\n");
  printf("delete employee_number\n");
  printf("type end to stop\n\n");

  printf("example: insert mike accounting 123 123456\n\n");

  while(running){
    printf("ready\n\n");

    // get user input
    fgets(buffer, BUFSIZ, stdin);
    strtok(buffer, "\n");

    if (strncmp(buffer, "end", 3) == 0) {
      running = 0;
    }

    // copy command buffer into msg struct
    msg_data.type = 1;
    strcpy(msg_data.text, buffer);

    // send data
    if (msgsnd(admin_msgid, (void *)&msg_data, BUFSIZ, 0) == -1) {
      fprintf(stderr, "msgsnd failed\n");
      exit(EXIT_FAILURE);
    }

    printf("waiting for server response\n\n");

    if (running) {
      // get response from the server
      if (msgrcv(client_msgid, (void *)&msg_data, BUFSIZ, 0, 0) == -1) {
        fprintf(stderr, "msgrcv failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
      }

      printf("server response: \n%s\n\n", msg_data.text);
    }
  }

  if (msgctl(admin_msgid, IPC_RMID, 0) == -1) {
    fprintf(stderr, "msgctl(IPC_RMID) failed\n");
    exit(EXIT_FAILURE);
  }

  if (msgctl(client_msgid, IPC_RMID, 0) == -1) {
    fprintf(stderr, "msgctl(IPC_RMID) failed\n");
    exit(EXIT_FAILURE);
  }
}
