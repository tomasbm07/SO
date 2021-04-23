//gcc main.c -lpthread -D_REENTRANT -Wall -o horse_track

#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdbool.h>

int shmid;
int *shared_var;
sem_t mutex;

// creates mutex for threds
pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;

/*----------------------------------*/
void client();
void bookie();
/*----------------------------------*/


int main(int argc, char *argv[]){
    int i;

    printf("Defined parameters:\n");
    printf("Number of horses per race: %s\n", argv[1]);
    printf("Bet value: %s\n", argv[2]);
    printf("Number of bettors: %s\n", argv[3]);
    printf("Number of races: %s\n", argv[4]);
    printf("Max bet value: %s\n", argv[5]);
    printf("Initial cash: %s\n", argv[6]);

    // Create shared memory
	if((shmid = shmget(IPC_PRIVATE, sizeof(shared_var), IPC_CREAT|0700)) == -1){
		printf("Error in shmget!\n");
		exit(1);
	}

	// Attach shared memory
	if((shared_var = shmat(shmid, NULL, 0)) == -1){
		printf("Error in shmat!\n");
		exit(1);
	}

    // Create mutex semaphore for processes
	if (sem_init(&mutex, 0, 1) == -1){
		printf("Erro a criar o semafro!\n");
		exit(1);
	}

    //fork into Client and Bookie
    if (fork() == 0){ // child / Client
        printf("Started client proces\n");
        client();
    } else { // parent / Bookie
        printf("Started bookie process\n");
        bookie();
    }

    // Wait for both processes
	for (i = 0; i < 2; i++){
		wait(NULL);
	}

    // remove resources
	shmctl(shmid, IPC_RMID, NULL);
    pthread_mutex_destroy(&mutex);

    exit(0);
}

void client(){
    printf("");
}

void bookie(){
    printf("");
}