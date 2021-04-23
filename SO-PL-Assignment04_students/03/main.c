#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h> // include POSIX semaphores
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>


int *shared_var;
int shmid;
bool run = true;
sem_t *mutex;
sem_t *empty;
sem_t *full; 

void producer(){
    while (1){
        usleep(100000 + rand() % 11);
        sem_wait(&empty);
        sem_wait(&mutex);
        shared_var = rand() % 49;
        sem_post(&mutex);
        sem_post(&full);
    }
}

void consumer(int num){
    FILE *f;
    char filename[30] = "";
    char buffer[30] = "";
    int i, numbers[6];

    sprintf(filename, "key_%d.txt", num);
    f = fopen(filename, "w");

    //read 6 numbers from shared variable
    for (i = 0; i < 6; i++){
        wait(&full);
        sem_wait(&mutex);
        numbers[i] = *shared_var;
        sem_post(&mutex);
        sem_post(&empty);
    }

    //write to file
    for (i = 0; i < 6; i++){
        sprintf(buffer, "%d ", numbers[i]);
        fputs(buffer, f);
    }

    fclose(f);
}

void remove_resources(){
    sem_close(mutex);
    sem_close(full);
    sem_close(empty);
    sem_unlink("MUTEX");
    sem_unlink("EMPTY");
    sem_unlink("FULL");
    shmctl(shmid, IPC_RMID, NULL);
}

int main(int argc, char *argv[]){
    srand(time(NULL));

    int i, n_procs;

    if (argc != 2){
        printf("Wrong number of parameters\n");
        exit(0);
    }

    n_procs = atoi(argv[1]);

    // Create shared memory
    if ((shmid = shmget(IPC_PRIVATE, sizeof(shared_var), IPC_CREAT | 0700)) == -1){
        printf("Error in shmget!\n");
        exit(1);
    }

    // Attach shared memory
    if ((shared_var = shmat(shmid, NULL, 0)) == -1){
        printf("Error in shmat!\n");
        exit(1);
    }


    // Create semaphore
    //sem_unlink("MUTEX");
    if ((mutex = sem_open("MUTEX", O_CREAT|O_EXCL, 0700, 1)) == -1){
        printf("Erro a criar o semafro!\n");
        exit(1);
    }
    //sem_unlink("EMPTY");
    if ((empty = sem_open("EMPTY", O_CREAT|O_EXCL, 0700, 1)) == -1){
        printf("Erro a criar o semafro!\n");
        exit(1);
    }
    //sem_unlink("FULL");
    if ((full = sem_open("FULL", O_CREAT|O_EXCL, 0700, 1)) == -1){
        printf("Erro a criar o semafro!\n");
        exit(1);
    }


    // initialize variable in shared memory
    *shared_var = 1;

    //Create producer
    if (fork() == 0){
        producer();
    }

    //create consumers
    for (i = 0; i < n_procs; i++){
        if (fork() == 0){ //child
            printf("Consumer %d created\n", getpid());
            consumer(i+1);
            printf("Consumer Finished\n");
            exit(0);
        }
    }

    // Wait for all worker processes
    for (i = 0; i < n_procs; i++){
        wait(NULL);
    }

    // remove resources (shared memory and semaphore)
    remove_resources();

    exit(0);
}
