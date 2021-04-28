// Operating Systems - University of Coimbra 2021
//
// A process generates messages with random priorities, from 1 to 3, being 1 the highest priority
// One process only gets messages of the highest priority (1)
// One process gets messages of any priority but choosing the higher priorities first
// One process gets messages by the order they were created
// All messages are sent and received using a MSQ

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

typedef struct {
  long priority; // Message type 
  int msg_number; // Payload 
} priority_msg;

int mqid; /* Message queue id */


void proc_only_highest_priority() {
  priority_msg received_msg;
  while(1){
    /*Only receive messages that have the maximum (highest) priority */
    msgrcv(mqid, &received_msg, sizeof(priority_msg), 1, 0);

    printf("[only_highest_priority] Received message [%d]!\n",received_msg.msg_number);
    sleep(5);
  }
}


void proc_high_priority_first() {
  priority_msg received_msg;
  while(1){
    /*Receive messages with the higher priority available */
    msgrcv(mqid, &received_msg, sizeof(priority_msg), -3, 0);

    printf("[high_priority_first] Received message [%d]!\n",received_msg.msg_number);
    sleep(5);
  }
}


void proc_by_order() {
  priority_msg received_msg;
  while(1){
    msgrcv(mqid, &received_msg, sizeof(received_msg), 0, 0);

    printf("[by_order] Received message [%d]!\n",received_msg.msg_number);
    sleep(5);
  }
}


void cleanup() {
	printf("\n");
	msgctl(mqid, IPC_RMID, 0);

	/* Guarantees that every process receives a SIGTERM , to kill them */
	kill(0, SIGTERM);
	exit(0);
}


int main() {
  int new_priority, counter=0;
  priority_msg my_msg;

  /* Redirect SIGINT to cleanup */
  signal(SIGINT, cleanup);

  /* Seed for a new sequence of pseudo-random integers to be returned by random() */
  srandom(getpid());

  if ( (mqid = msgget(IPC_PRIVATE, IPC_CREAT|0777)) < 0 ){
  	perror("Creating message queue");
  	exit(-1);
   }

  printf("Press ^C to finish...\n\n");

  if (fork() == 0) proc_only_highest_priority();
  if (fork() == 0) proc_high_priority_first();
  if (fork() == 0) proc_by_order();

  while(1){
    new_priority = random() % 3 + 1;
    counter++;
    printf("=> [Message generator] Message %d has priority %d\n", counter,new_priority);

    my_msg.priority = new_priority;
    my_msg.msg_number = counter;

    /* Send message */
    msgsnd(mqid, &my_msg, sizeof(priority_msg), 0);

    sleep(1);  // new message generated every second
  }
  return 0;
}
