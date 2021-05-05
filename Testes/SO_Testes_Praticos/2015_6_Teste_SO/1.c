#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>


#define TOTAL_MATRICES 5


typedef struct mem_shared{
	int matrix[10][10];
	int n_processed;
	int matrix_number;
	int finished_workers;
} mems;


mems* mems_ptr;
int shmid;
sem_t *mutex, *gerador_sem;


void cleanup(){
	printf("\nCleaning up...\n");

	shmdt(mems_ptr);
	shmctl(shmid, IPC_RMID, NULL);

	sem_close(mutex);
	sem_close(gerador_sem);

	sem_unlink("MUTEX");
	sem_unlink("GERADOR");

	exit(0);
}


void signal_handler(){
	cleanup();
}


void gerador(){
	//printf("[%d] Gerador created\n", getpid());
	int hehexD = 0;

	while (hehexD < TOTAL_MATRICES){
		sem_wait(mutex);
		mems_ptr->finished_workers = 0;
		for (int i = 0; i < 10; i++){
			for (int j = 0; j < 10; j++){
				mems_ptr->matrix[i][j] = rand() % 10;
			}
		}
		(mems_ptr->matrix_number)++;
		sem_post(mutex);

		printf("[%d] New matrix created\n", getpid());
		sem_wait(gerador_sem);	
		hehexD++;
	}

	printf("[%d] Gerador Finished!\n", getpid());
	exit(0);
}


void contadores(int num){
	int counter;
	//printf("[%d - Counting %d's] Worker created\n", getpid(), num);

	sleep(1);

	int matrix_counter = 0;

	while(matrix_counter < TOTAL_MATRICES){
		counter = 0;
		sleep(1);
		sem_wait(mutex);
		if (matrix_counter != mems_ptr->matrix_number){
			matrix_counter = mems_ptr->matrix_number;

			for (int i = 0; i < 10; i++){
				for (int j = 0; j < 10; j++){
					if (mems_ptr->matrix[i][j] == num)
						counter ++;
				}
			}
			sem_post(mutex);

			printf("[%d - Counting %d's] Round %d, got %d.\n", getpid(), num, matrix_counter, counter);

			sem_wait(mutex);
			(mems_ptr->finished_workers)++;
			if (mems_ptr->finished_workers == 10) {
				sem_post(gerador_sem);
				printf("-------------------------\n");
			} else{
				sem_post(mutex);
			}

		} else {
			sem_post(mutex);
		}
		sem_post(mutex);

	}

	exit(0);
}


int main(int argc, char const *argv[]){
	//inicializaçao
	srand(time(NULL));

	signal(SIGINT, signal_handler);

	sem_unlink("MUTEX");
	mutex = sem_open("MUTEX", O_CREAT | O_EXCL | 0700, 1, 1);
	if (mutex < 0)
		printf("ERRO: SEM MUTEX");

	sem_unlink("GERADOR");
	gerador_sem = sem_open("GERADOR", O_CREAT | O_EXCL | 0700, 1, 0);
	if (gerador_sem < 0)
		printf("ERRO: SEM GERADOR");

	//criaçao e mapeamento da memoria partilhada
	if ((shmid = shmget(IPC_PRIVATE, sizeof(mems), IPC_CREAT | IPC_EXCL |0700)) < 0)
		printf("ERRO: SHMGET\n");

	if ((mems_ptr = shmat(shmid, NULL, 0)) < 0)
		printf("ERRO: SHMAT\n");

	//Inicializar a memoria
	mems_ptr->n_processed = 0;
	mems_ptr->matrix_number = 0;
	mems_ptr->finished_workers = 0;

	//Criar o processo gerador e os processos que vao efetuar a contagem
	//(cada processo vai contar  um numero 0-9)
	if(fork() == 0)
		gerador();
	
	for (int i = 0; i < 10; i++){
		if (fork() == 0){
			contadores(i);
		}
	}

	for (int i = 0; i < 11; i++) wait(NULL);

	//Limpar recursos
	cleanup();
	
	exit(0);
}