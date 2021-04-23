//gcc main.c -lpthread -D_REENTRANT -Wall -o cinema

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

#define N_OFFICES 2
#define CINEMA_SIZE 150

pthread_t my_threads[N_OFFICES];

// creates mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// cinema seats
int seats[CINEMA_SIZE];

bool run = true;


void *worker();
void *session_controller();
void print_seats();


int main(){
    srand(time(NULL));
    int i;

    for(i = 0; i < CINEMA_SIZE; i++) seats[i] = 0;

    //worker threads
    for(i = 0; i < N_OFFICES; i++) 
        pthread_create(&my_threads[i], NULL, worker, NULL);

    //cinema thread
    pthread_create(&my_threads[i + 1], NULL, session_controller, NULL);

    //wait for threads to finish
	for (i = 0; i < N_OFFICES + 2; i++){
		pthread_join(my_threads[i], NULL);
	}

    printf("Get ready, the show is starting!\n");
    printf("Final display:\n");
    print_seats();

    //destroy the mutex
	pthread_mutex_destroy(&mutex);

    exit(0);
}

void *worker(void){
    int x, num;
    while(run){
        for(x = 0; x < 3; x++){
            num = rand() % CINEMA_SIZE;
            pthread_mutex_lock(&mutex);
            if (seats[num] != 1){
                seats[num] = 1;
            }
        pthread_mutex_unlock(&mutex);
        }
    sleep(1 + rand() % 2); // sleep 1-2s
    }
    pthread_exit(NULL);
}

void *session_controller(){
    int time = 0;
    while(time != 6){
        print_seats();
        sleep(10);
        time++;
    }
    run = false;
    pthread_exit(NULL);
}

void print_seats(){
    int x;
    pthread_mutex_lock(&mutex);

    for(x = 0; x < sizeof(seats)/sizeof(int); x++){
        if (x % 10 != 0){
            printf("%d ", seats[x]);
        }
        else{
            printf("\n");
        }
    }

    pthread_mutex_unlock(&mutex);
    printf("\n");
}