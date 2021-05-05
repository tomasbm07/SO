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


//variáveis globais
int fd_named_pipes[2];
int soma_total = 0;
int nums_read = 0;
pthread_t threads[2];
fd_set read_set;
int value;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void *reader();
void *estatisticas();
void cleanup();


int main(int argc, char const *argv[]) {

	signal(SIGINT, cleanup);

	unlink("PIPE1");
	unlink("PIPE2");

	if (mkfifo("PIPE1", O_CREAT | O_EXCL | 0600) < 0){
		printf("Erro a criar o PIPE1\n");
		exit(1);
	}

	if (mkfifo("PIPE2", O_CREAT | O_EXCL | 0600) < 0){
		printf("Erro a criar o PIPE2\n");
		exit(1);
	}

	if ((fd_named_pipes[0] = open("PIPE1", O_RDWR)) < 0) {
    	printf("Erro a criar o PIPE1\n");
    	exit(1);
  	}

  	if ((fd_named_pipes[1] = open("PIPE2", O_RDWR)) < 0) {
    	printf("Erro a criar o PIPE1\n");
    	exit(1);
  	}

  	//criar as threads
  	pthread_create(&threads[0], NULL, reader, NULL);

  	pthread_create(&threads[1], NULL, estatisticas, NULL);

  	//esperar pelas threads
  	for (int i = 0; i < 2; i++) pthread_join(threads[i], NULL);

  	cleanup();

  	exit(0);
}


void *reader(){
	printf("Reader ready...\n");

	while(1){
		FD_ZERO(&read_set);
		FD_SET(fd_named_pipes[0], &read_set);
		FD_SET(fd_named_pipes[1], &read_set);

		if(select(fd_named_pipes[1] + 1, &read_set, NULL, NULL, NULL) > 0){

			if (FD_ISSET(fd_named_pipes[0], &read_set)){
				read(fd_named_pipes[0], &value, sizeof(int));
				printf("[Received %d from PIPE1]\n", value);
				pthread_mutex_lock(&mutex);
				soma_total += (int)(value);
				nums_read++;
				pthread_mutex_unlock(&mutex);
			}

			if (FD_ISSET(fd_named_pipes[1], &read_set)){
				read(fd_named_pipes[1], &value, sizeof(int));
				printf("[Received %d from PIPE2]\n", value);
				pthread_mutex_lock(&mutex);
				soma_total += value;
				nums_read++;
				pthread_mutex_unlock(&mutex);
			}

		}


	}
	pthread_exit(NULL);
}


void *estatisticas(){
	printf("Estatisticas ready...\n");
	while(1) {
		sleep(5);
		pthread_mutex_lock(&mutex);
		printf("soma_total = %d\n", soma_total);
		printf("nums_read = %d\n", nums_read);
		pthread_mutex_unlock(&mutex);
	}

	pthread_exit(NULL);
}


void cleanup(){
	for (int i = 0; i < 2; i++) close(fd_named_pipes[i]);

	unlink("PIPE1");
	unlink("PIPE2");

	exit(0);
}