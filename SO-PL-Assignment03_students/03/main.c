#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>


int main(void){
	int flag = 0

	for(int i = 0; i < 4; i++){
		if (fork() == 0){ // child
			break;
			sleep((10 - 2*(i+1)));
			printf("I'm %d and my father is %d\n", getpid(), getppid());
			exit(0);	
			

		} else{ //parent
			sleep(20);
			exit(0);
		}
	}

	printf("I'm the original: %d", getpid());

	return 0;
}



