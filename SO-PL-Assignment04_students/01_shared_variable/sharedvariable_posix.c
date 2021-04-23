//compile with: gcc -Wall -pthread sharedvariable_posix.c -o svar
//using POSIX named semaphores

#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h> // include POSIX semaphores
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/wait.h>

/*insert here the other libraries needed*/

int *shared_var;
int shmid;
sem_t *mutex;

void worker(){
	usleep(1000000 + rand() % 11 * 100000); //uses microseconds sleep and waits 1 to 2 seconds in periods of 0.1 secs

	sem_wait(&mutex);
	(*shared_var)++;
	sem_post(&mutex);
}

int main(int argc, char *argv[]){
	int i, n_procs;

	if (argc != 2){
		printf("Wrong number of parameters\n");
		exit(0);
	}

	n_procs = atoi(argv[1]);

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

	// Create semaphore
	if (sem_init(&mutex, 0, 1) == -1){
		printf("Erro a criar o semafro!\n");
		exit(1);
	}

	// initialize variable in shared memory
	*shared_var = 1000;

	// Create worker processes
	for (i = 0; i < n_procs; i++){
		if(fork() == 0){ //child
			printf("Worker %d created!\n", getpid());
			worker();
			printf("Worker Finished!\n");
			exit(0);
		} 
	}

	// Wait for all worker processes
	for (i = 0; i < n_procs; i++){
		wait(NULL);
	}

	// print final result
	printf("Final result: %d\n", *shared_var);

	// remove resources (shared memory and semaphore)
	sem_destroy(mutex);
	shmctl(shmid, IPC_RMID, NULL);
	

	exit(0);
}