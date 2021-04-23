#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void worker()
{
  printf("[%d] Let's see who my dady is: %d\n", getpid(), getppid());
  sleep(10);
  printf("[%d] Let's see who my dady is: %d\n", getpid(), getppid());
}

int main()
{
  int i;
  for (i = 0; i < 10; i++)
  {
    if (fork() == 0)
    {
      worker();
      exit(0);
    }
  }

  printf("[%d] Big dady is going away!\n", getpid());
  sleep(1);
  return 0;
}
