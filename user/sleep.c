#include "kernel/types.h"
#include "user/user.h"


int
main(int argc, char *argv[])
{
  int i, j;
  int s;
  char c;

  if (argc < 2) {
    printf("sleep: missing operand\n");
    exit(-1);
  }

  for (i = 1; i < argc; i++) {
    for (j = 0; j < strlen(argv[i]); j++) {
      c = argv[i][j];
      if (c < '0' || c > '9') {
        printf("sleep: invalid number '%s'\n", argv[i]);
        exit(-1);
      }
    }

    s = atoi(argv[i]);
    sleep(s);
  }

  exit(0);
}
