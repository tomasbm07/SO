#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h> // include POSIX semaphores
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>


int *shared_var;
int shmid;
sem_t *mutex, *full;

void producer(){
    while (1){
        srand (time(NULL));
        sem_wait(mutex);
        *shared_var = rand() % 49;
        printf("Generated: %d\n", *shared_var);
        sem_wait(mutex);
    }
}

void consumer(){
    int i;
    for(i = 0; i < 5; i++){
        printf("%d: Got %d\n", getpid(), *shared_var);

    }
    exit(0);
}

void remove_resources(){
    sem_close(mutex);
    sem_close(full);
    sem_unlink("MUTEX");
    sem_unlink("FULL");
    shmctl(shmid, IPC_RMID, NULL);
}

int main(int argc, char *argv[]){

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
    sem_unlink("MUTEX");
    if ((mutex = sem_open("MUTEX", O_CREAT|O_EXCL, 0700, 1)) == -1){
        printf("Erro a criar o semafro!\n");
        exit(1);
    }
    sem_unlink("FULL");
    if ((full = sem_open("FULL", O_CREAT|O_EXCL, 0700, 1)) == -1){
        printf("Erro a criar o semafro!\n");
        exit(1);
    }

    // initialize variable in shared memory
    *shared_var = 1;

    if (fork() == 0){
        producer();
    }

    //Create producer
    if (fork() == 0){
        for (i = 0; i < n_procs; i++){
            if (fork() == 0){ //child
                printf("Consumer %d created\n", getpid());
                //consumer();
                printf("Consumer Finished\n");
            }
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
