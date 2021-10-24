#include "kernel/types.h"
#include "user/user.h"


int main()
{
  int p[2], pid;
  char tmp;

  if (pipe(p) < 0) {
    fprintf(2, "Create pipeline fail.\n");
    exit(-1);
  }

  pid = fork();
  if (pid < 0) {
    fprintf(2, "fork() fail.\n");
    exit(-1);
  }

  if (pid == 0) {
    read(p[1], &tmp, 1);
    printf("%d: received ping\n", getpid());
    write(p[0], "1", 1);
  } else {
    write(p[0], "1", 1);
    read(p[1], &tmp, 1);
    wait(0);
    printf("%d: received pong\n", getpid());
  }

  exit(0);
}
