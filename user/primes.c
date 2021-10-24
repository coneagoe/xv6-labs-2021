#include "kernel/types.h"
#include "user/user.h"

#define TRUE 1
#define FALSE 0

int is_prime(int n)
{
  int i;

  if (n == 0 || n == 1)
    return FALSE;
  else {
    for (i = 2; i <= n/2; i++) {
      if (n % i == 0)
        return FALSE;
    }
  }

  return TRUE;
}

int main()
{
  int p[2], tmp, i;
  int children_pid[34] = {0};

  pipe(p);
  for (i = 2; i < 36; i++) {
    if (is_prime(i)) {
    }
  }

  exit(0);
}
