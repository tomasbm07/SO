#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX 60    // game width
#define SIZE 10   // road width
#define LINES 40  // max game lines

int* shared_pos;
int shmid;
int direction = 1;
int left = 0;
void road();
void signals(int signum);
void ignore_signals(int signum);

int main() {
  int pid;
  struct sigaction act;

  // Create shared memory for car position
  if ((shmid = shmget(IPC_PRIVATE, sizeof(shared_pos), IPC_CREAT | 0700)) == -1) {
    printf("Error in shmget!\n");
    exit(1);
  }

  if((shared_pos = shmat(shmid, NULL, 0)) == -1){
		printf("Error in shmat!\n");
		exit(1);
	}

  // initial car position
  *shared_pos = SIZE / 2;

  // Create child to handle road
  pid = fork();
  if (pid == 0) {
    road();
    exit(0);
  }

  //*** Father ***//
  // Handle signals

  signal(SIGINT, signals);
  signal(SIGTSTP, signals);

  // Wait for child
  wait(NULL);

  /* TO COMPLETE - Wait for child - if a signal is received while waiting, the
   * wait must continue */

  printf("Child finished!\n");

  // Remove shared memory
  shmdt(shared_pos);
  shmctl(shmid, IPC_RMID, NULL);

  exit(0);
}

void road() {
  struct sigaction act;

  // Ignore SIGINT and SIGTSTP
  // Configuring sigaction
  act.sa_handler = SIG_IGN;

  sigaction(SIGINT, &act, 0);
  sigaction(SIGTSTP, &act, 0);


  for (int i = 0; i < LINES; i++) {
    if ((*shared_pos) > left && (*shared_pos) < left + SIZE - 1) {
      printf("%*s>%*sV%*s<\n", left, "", (*shared_pos) - left - 1, "", SIZE + left - 2 - (*shared_pos), "");
    } else {
      printf("%*cCRASH! GAME OVER!!!\n", left, ' ');
      exit(0);
    }

    if (direction == 1) {
      left++;
    } else {
      left--;
    }

    if (left + SIZE >= MAX) {
      direction = 0;
    } else if (left <= 0) {
      direction = 1;
    } else {
      // Make random direction change
      if (rand() % 10 == 0) {
        if (direction == 0)
          direction = 1;
        else
          direction = 0;
      }
    }
    usleep(500 * 1000);
  }
  printf("END OF ROAD! WELL DONE!!!\n");
}

void signals(int signum) {
	if (signum == SIGINT){
		//increase position = move right
		//printf("\nGot SIGINT - Moving right\n");
		(*shared_pos)++;
	} else if(signum == SIGTSTP){
		//decrease position = move left
		//printf("\nGot SIGTSTP - Moving left\n");
		(*shared_pos)--;
	}

  printf("\n");
}

