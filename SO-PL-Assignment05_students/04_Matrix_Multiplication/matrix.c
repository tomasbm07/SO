// Compile as: gcc matrix.c -lpthread -D_REENTRANT -Wall -o matrix

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define M 3 //first matrix lines
#define K 2 //first matrix columns; second matrix lines
#define N 3 //second matrix columns

typedef struct v{
	int row;
	int column;
} coordenate;

void *worker(void *coord);
void *show_matrix(); // prints matrices on screen

pthread_t my_thread[(M * N) + 1];
int A[M][K] = {{1, 4}, {2, 5}, {3, 6}};
int B[K][N] = {{8, 7, 6}, {5, 4, 3}};
int C[M][N];

// creates mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void print_matrix(){
	// prints matrices on screen when having exclusive access
	pthread_mutex_lock(&mutex);
	printf(" -------\n");
	int x, y;
	for (x = 0; x < M; x++){
		printf(" ");
		for (y = 0; y < N; y++){
			printf("%d  ", C[x][y]);
		}
		printf("\n");
	}
	printf(" -------\n");

	pthread_mutex_unlock(&mutex);
}

int main(void){
	int x, y;
	coordenate *xD;

	// create threads
	printf("Creating threads!!\n");
	for (x = 0; x < M; x++){
		for (y = 0; y < N; y++){
			xD = malloc(sizeof(coordenate));
			xD->row = x;
			xD->column = y;
			pthread_create(&my_thread[x+y], NULL, worker, (void*)xD); // workers
		}
	} 
	pthread_create(&my_thread[x + y + 1], NULL, show_matrix, NULL); //printing thread
	printf("DONE creating threads!!\n");

	//wait for threads to finish
	for (x = 0; x < (M * N) + 2; x++){
		pthread_join(my_thread[x], NULL);
	}

	// prints final results of the calculation and leaves
	printf("Final matrix is:\n");
	print_matrix();

	//destroy the mutex
	pthread_mutex_destroy(&mutex);

	exit(0);
}

void *worker(void *coord){
	// calculates each matrix element in the coordinate received
	sleep(1 + rand() % 10); //random sleep
	int i = (((coordenate*)coord)->row), j = (((coordenate*)coord)->column), k;
	pthread_mutex_lock(&mutex);
	//printf("%d %d\n", i, j);
		for (k = 0; k < M; ++k) {
        	C[i][j] += A[i][k] * B[k][j];
		}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void *show_matrix(){
	// prints the status of the matrix in each second, along a period of 10 seconds
	long start = time(NULL), end_time = start + 10;
	while(start < end_time){
		sleep(1);
		print_matrix();
		start = time(NULL);
	}
	pthread_exit(NULL);
}
