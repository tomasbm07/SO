// Signal Receiver - @2019
// Block, ignore and handle signals
// Uses sigaction and sigprocmask

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

void signals(int signum);

int main() {
	int n;
	time_t now;
	struct tm timenow;
	struct sigaction action;
	sigset_t new_mask, orig_mask;

	// Ignore ^C and ^Z (SIGINT and SIGTSTP) using sigaction
	sigaction(SIGINT, NULL, NULL);
	sigaction(SIGTSTP, &action, NULL);

	// Handle SIGUSR1 and SIGUSR2
	/* TO COMPLETE - Handle SIGUSR1 and SIGUSR2 - both handled by function signals() */
	signal(SIGUSR1, signals);
	signal(SIGUSR2, signals);

	// Alternate blocking and responding to both SIGUSR1 and SIGUSR2
	// Add SIGUSR1 and SIGUSR2 to &new_mask
	//(adiciona estes signals á struct para depois serem bloqueados pelo sigprogmask() )
	sigemptyset(&new_mask);
	sigaddset(&new_mask,SIGUSR1);
	sigaddset(&new_mask,SIGUSR2);

	printf("My PID is: %d\n\n",getpid());

	// originally neither SIGUSR1 or SIGUSR2 are blocked
	// Block SIGUSR1 and SIGUSR2 and get previous mask (was the original)
	time(&now);
	localtime_r(&now,&timenow);
	printf("%02d:%02d:%02d - Blocking SIGUSR1 and SIGUSR2\n",timenow.tm_hour,timenow.tm_min,timenow.tm_sec);

	//Block signals
	sigprocmask(SIG_BLOCK, &new_mask, &orig_mask);

	while(1){
		n=10;
		while(n!=0) n=sleep(n);	// repeat if sleep was interrupted by a signal
		time(&now);
		localtime_r(&now,&timenow);
		printf("%02d:%02d:%02d - Unblocking SIGUSR1 and SIGUSR2\n",timenow.tm_hour,timenow.tm_min,timenow.tm_sec);

 		/* TO COMPLETE - Unblock SIGUSR1 and SIGUSR2 */
		sigprocmask(SIG_SETMASK, &orig_mask, NULL);

		n=10;
		while(n!=0) n=sleep(n);	// repeat if sleep was interrupted by a signal
		time(&now);
		localtime_r(&now,&timenow);
		printf("%02d:%02d:%02d - Blocking SIGUSR1 and SIGUSR2\n",timenow.tm_hour,timenow.tm_min,timenow.tm_sec);

 		/* TO COMPLETE - Block SIGUSR1 and SIGUSR2 */
		sigprocmask(SIG_BLOCK, &new_mask, &orig_mask);
		}
}

void signals(int signum) {
	if(signum == SIGUSR1) {
		printf("SIGUSR1 received\n");
	} else if(signum == SIGUSR2) {
		printf("SIGUSR2 received\n");
	}
}
