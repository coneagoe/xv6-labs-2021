#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAX_CMD_NAME_LEN 256

struct execcmd {
  int  argc;
  char *argv[MAXARG];
};


int option_n = 0;
struct execcmd gcmd = {0};


void split_args(const char * const arg_str, int *argc, char ***argv)
{
  const char *cur_n = arg_str;
  const char *pre_n = arg_str;
  const char *arg_end = arg_str + strlen(arg_str);
  char *p_arg;
  int arg_len, end = 0, find = 0;

  *argc = 0;
  *argv = malloc(sizeof(char *) * MAXARG);

  while (1) {
    if (*argc >= MAXARG)
      break;

    if (cur_n > arg_end) {
      end = 1;
      find = 1;
    } else {
      if (option_n) {
        if (*cur_n == '\\' && *(cur_n + 1) == 'n')
          find = 1;
      } else {
        if (*cur_n == '\n')
          find = 1;
      }
    }

    if (find) {
      arg_len = cur_n - pre_n;
      (*argv)[*argc] = malloc(arg_len + 1);
      p_arg = (*argv)[*argc];
      memmove(p_arg, pre_n, arg_len);
      p_arg[arg_len] = 0;
      *argc += 1;
    }

    if (end)
      break;

    if (find) {
      if (option_n) {
        pre_n = cur_n + 2;
        cur_n += 2;
      } else {
        pre_n = cur_n + 1;
        cur_n += 1;
      }
    } else {
      cur_n += 1;
    }

    find = 0;
  }
}


void clean(int argc, char ***argv)
{
  int i;

  for (i = 0; i < argc; i++) {
    free((*argv)[i]);
  }

  free(*argv);
}


void parsecmd(int argc, char *argv[])
{
  int cmd_index, argv_start_index;
  int i, j;

  if (argc < 2) {
    exit(0);
  }

  if (strcmp(argv[1], "-n") == 0)
    option_n = 1;

  if (option_n) {
    cmd_index = 3;
    argv_start_index = 4;
  }
  else {
    cmd_index = 1;
    argv_start_index = 2;
  }

  gcmd.argv[0] = argv[cmd_index];
  for (i = argv_start_index, j = 1; i < argc; i++, j++) {
    if (i >= MAXARG) {
      fprintf(2, "The argument is too long!\n");
      exit(-1);
    }

    gcmd.argv[j] = argv[i];
  }
  gcmd.argc = j;
}


void xargs_exec(char *cmd, char **args)
{
  if (fork() == 0)
    exec(cmd, args);
  else
    wait(0);
}


void xargs(struct execcmd *cmd)
{
  int i, n, argc;
  char line_buf[MAX_CMD_NAME_LEN] = {0}, input[MAX_CMD_NAME_LEN] = {0};
  char *p = input;
  char **argv;
  struct execcmd *pcmd;

  memset(input, 0, sizeof(input));
  while (1) {
    memset(line_buf, 0, sizeof(line_buf));
    n = read(0, line_buf, sizeof(line_buf));
    if (n <= 0) {
      break;
    }

    strcpy(p, line_buf);
    p += strlen(line_buf);
  }

  split_args(input, &argc, &argv);

  for (i = 0, pcmd = cmd; i < argc; i++) {
    pcmd->argv[pcmd->argc] = argv[i];
    xargs_exec(pcmd->argv[0], pcmd->argv);
  }

  clean(argc, &argv);
}


int main(int argc, char *argv[])
{
  parsecmd(argc, argv);
  xargs(&gcmd);
  exit(0);
}
