// compile as gcc -pthread -Wall process_tasks.c -o ptasks

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 10

int processing_list[N];
int next_request=0;
pthread_t my_thread[N];
sem_t mutex;

void *worker(void* p) {
  int w_id=*((int *)p);
  pthread_t id=pthread_self();

  printf("Thread %ld: Worker %d starting\n",id,w_id);

  sem_wait(&mutex);
  printf("Thread %ld: Worker %d is going to execute order %d\n",id, w_id, next_request);
  processing_list[next_request]=w_id; //save the worker that took the order
  sched_yield(); // force thread to yield the processor - just to simulate processor scheduling
  next_request++; //update next request number
  sem_post(&mutex);

  usleep(10); // simulates the processing of the order
  printf("Thread %ld: Worker %d finishing\n",id,w_id);
  pthread_exit(NULL);
}

int main(void) {
  int i, worker_id[N];
  
  for (i = 0; i < N; i++) processing_list[i]=-1; //initialize processing_list array


  // Create a semaphore
	if (sem_init(&mutex, 0, 1) == -1){
	  printf("Erro a criar o semafro!\n");
		exit(1);
	}
  
  // create N threads
  for (i = 0; i < N; i++) {
    worker_id[i]=i+1;
    pthread_create(&my_thread[i], NULL, worker, &worker_id[i]);
  }

  //wait for all threads to finish work
  for (i = 0; i < N; i++) {
    pthread_join(my_thread[i], NULL);
  }

  printf("Final list -----\n");
  for (i = 0; i < N; i++) printf(" %d => worker %d ;\n",i,processing_list[i]);

  sem_destroy(mutex);

  exit(0);
}
