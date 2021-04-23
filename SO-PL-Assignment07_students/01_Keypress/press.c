#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int t_ini, num;

/* TO COMPLETE: Create the function that handles CTRL-Z */
void closeProgram(int signal){
	char aux;
	printf("\nPressed CTRL-Z: Do you want to close the program? [y/n]\n");
	scanf("%c", &aux);
	if (aux == 'y'){
		printf("Program closing\n");
		exit(0);
	}
	else{

	}
}


/* TO COMPLETE: Create the function that handles CTRL-C */
void guessHandler(int signal){
	int guess = time(NULL) - t_ini;
	printf("\nPressed CTRL-C to make guess\n");
	if (guess == num){
		printf("You guessed right!\n");
	} else {
		printf("You guessed wrong! You guessed: %d\n", guess);
	}
	
}

void ignoreSignal(int sig){
	printf("\nPressed CTRL-C: ignored!\n");
}


int main(){
  /* TO COMPLETE: Redirect signals */
  signal(SIGINT, ignoreSignal);
  signal(SIGTSTP, closeProgram);

  srand(time(NULL));

  while (1) {
	signal(SIGINT, ignoreSignal);
	num  = rand() % 8 + 2;
	sleep(5);
	printf("\n\nPress CTRL-C in %d seconds!\nCountdown starting in 2 seconds... Get Ready!!\n",num);
	sleep(2);
	printf("Start counting the seconds!!\n");
	signal(SIGINT, guessHandler);
	t_ini = time(NULL);
  	pause();
	}
  return 0;
}
