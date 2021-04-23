#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void worker() {
  printf("[%d] Hi, I'm a worker process! Going to die...\n", getpid());
}

int main() {
  int i;
  for (i=0; i<10; i++) {
    if (fork() == 0) {
      worker();
      exit(0);
    }
  }
  printf("[%d] Big father is sleeping!\n", getpid());
  sleep(10);

  return 0;
}
