#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>


#define PIPE_NAME "/home/user/my_pipe"

char str[256];
int fd_named_pipe, fd_temp[2], fd_hum[2];  // to save file descriptors
int temp_sum, temp_samples, hum_sum, hum_samples;  // to save temperature and humidity sum and number of samples
int value;
int pid[2];  // saves the pid of child processes

void shutdown_all();
void clean_resources();
void sigint(int signum);
void measure_temp();
void measure_humidity();


void clean_resources() { // cleans all resources including closing of files 
	close(fd_temp[0]);
	close(fd_temp[1]);
	
	close(fd_hum[0]);
	close(fd_hum[1]);

	close(fd_named_pipe); 
	unlink(PIPE_NAME);
}

void sigint(int signum) {  // handling of CTRL-C
	shutdown_all();
}

void shutdown_all() {
  kill(pid[0], SIGKILL);
  kill(pid[1], SIGKILL);
  while (wait(NULL) != -1);
  clean_resources();
  printf("\nBye bye\n");
  exit(0);
}

void measure_temp() {  // generates temperature values and writes them in the corresponding pipe
	//fechar a parte de leitura do pipe neste processo
	close(fd_temp[0]);
	close(fd_hum[0]);
	close(fd_hum[1]);

	while (1){
		int num = (rand() % 31) + 10 ; // 10ºC - 40ºC
		write(fd_temp[1], &num, sizeof(num));
		sleep(3);
	}

}

void measure_humidity() {
	//fechar a parte de leitura do pipe neste processo
	close(fd_hum[0]);
	close(fd_temp[0]);
	close(fd_temp[1]);
	while (1){
		int num = (rand() % 101); // 0% - 100%
		write(fd_hum[1], &num, sizeof(num));
		sleep(3);
	}
	
}

int main() {
  int number_of_chars;
  fd_set read_set;

  // Server terminates when CTRL-C is pressed
  // Redirect CTRL-C
  signal(SIGINT, sigint);

  // create pipes
  pipe(fd_temp);
  pipe(fd_hum);
  
  // create childs
  if ((pid[0] = fork()) == 0) {
    measure_temp();
    exit(0);
  }

  if ((pid[1] = fork()) == 0) {
    measure_humidity();
    exit(0);
  }


  // Creates the named pipe if it doesn't exist yet
  unlink(PIPE_NAME);
  if (mkfifo(PIPE_NAME, O_CREAT | O_EXCL | 0666) < 0){
	  perror("Erro a criar o pipe!\n");
	  exit(-1);
  }

  if ((fd_named_pipe = open(PIPE_NAME, O_RDWR)) < 0)
  	printf("error\n");

  // Listen to messages
  printf("Listening to all pipes!\n\n");

  close(fd_hum[1]);
  close(fd_temp[1]);

  while (1) {
    // I/O Multiplexing
	FD_ZERO(&read_set);
	FD_SET(fd_temp[0], &read_set);
	FD_SET(fd_hum[0], &read_set);
	FD_SET(fd_named_pipe, &read_set);

	if(select(fd_named_pipe+1, &read_set, NULL, NULL, NULL) > 0){

		if (FD_ISSET(fd_named_pipe, &read_set)) {
		number_of_chars = read(fd_named_pipe, str, sizeof(str));
		str[number_of_chars - 1] = '\0';  // put a \0 in the end of string

		if (strcmp(str, "AVG TEMP") == 0)
			printf("[SERVER Received \"%s\" command]\nAverage Temperature= %.2fºC\n",str, (double)temp_sum / temp_samples);
		else if (strcmp(str, "AVG HUM") == 0)
			printf("[SERVER Received \"%s\" command]\nAverage Humidity= %.2f %%\n",str, (double)hum_sum / hum_samples);
		else if (strcmp(str, "RESET") == 0) {
			printf("[SERVER received \"%s\" command]\nCounters reset!\n", str);
			temp_sum = 0;
			temp_samples = 0;
			hum_sum = 0;
			hum_samples = 0;
		} else if (strcmp(str, "SHUTDOWN") == 0) {
			printf("[SERVER received \"%s\" command]\nServer shutdown initiated!\n",str);
			shutdown_all();
		} else
			printf("[SERVER Received unknown command]: %s \n", str);
		}  // if(FD_ISSET(fd_named_pipe))
		if (FD_ISSET(fd_temp[0], &read_set)) {
		read(fd_temp[0], &value, sizeof(int));
		printf("[SERVER received new temperature]: %dºC\n", value);
		temp_sum += value;
		temp_samples++;
		}
		if (FD_ISSET(fd_hum[0], &read_set)) {
		read(fd_hum[0], &value, sizeof(int));
		printf("[SERVER received new humidity]: %d\n", value);
		hum_sum += value;
		hum_samples++;
		}
		/* TO COMPLETE */
	}
  }  // While(1)
  return 0;
}
