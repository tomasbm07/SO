#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
  if (execlp("ls", "ls", "-l", NULL) == -1)
    perror("Error executing ls: ");
  else
    printf("This cannot happen!\n");

  return 0;
}