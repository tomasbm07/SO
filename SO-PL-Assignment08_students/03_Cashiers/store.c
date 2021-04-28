#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#define CASHIERS_NUM 3

typedef struct {
	long mtype;
	int customer_id;
	int num_products;
} customer_msg;

int mq_id;
pid_t proc_ori;

void cleanup(int signum) {
	printf("\n\n[%d] Cleanup and exit!\n", getpid());

	/* TO COMPLETE: Cleanup - Remove MSQ */


	// End process
	exit(0);
}

// Generate customer requests
void generateCustomers() {
	printf("[%d] Store is now open!\n", getpid());

	srand(time(NULL));
	int customer_id = 0;

	while (1) {
		customer_msg customer;
		long mtype = rand()%CASHIERS_NUM+1; // Select between available cashiers (mtype>0)
		int num_products = rand()%3+2; // Products between 2 and 5

		/* TO COMPLETE: Create customer */

		/* TO COMPLETE: Print customer details */

		/* TO COMPLETE: Add customer to Message Queue */

		// Time between customers arrival
		sleep(rand()%5+1);

		//increment customer number
		customer_id++;
	}
}

// Cachiers process
void cashier(int i) {
	printf("[%d] Cashier %d open!\n", getpid(), i);

	customer_msg customer;

	while (1) {
		// Process next customer in cachier queue
		printf("[%d] Cashier %d waiting for next customer.\n", getpid(), i);

		/* TO COMPLETE: Get next customer from message queue */

		/* TO COMPLETE: Print customer details */

		/* TO COMPLETE: Process customer products */

	}
}

int main(int argc, char* argv[]) {
	pid_t p;
	int i;

	proc_ori=getpid(); // save the original proc PID so that only one destroys de MSQ

	/* TO COMPLETE: Handle ^C */

	/* TO COMPLETE: Create Message Queue */

	/* TO COMPLETE: Create cashiers processes */

	/* TO COMPLETE: Generate customer requests */
}
