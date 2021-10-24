#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


static void do_find(const char *path, const char *file_name);

void basename(const char * const path, char *buf)
{
  const char *p = path + strlen(path);

  while (!((p == path) || (*p == '/'))) {
    p--;
  }

  strcpy(buf, *p == '/' ? p+1 : p);
}


int is_dir(const char *path)
{
  int fd;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "cannot open %s\n", path);
    return 0;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "cannot state %s\n", path);
    close(fd);
    return 0;
  }

  close(fd);

  return(st.type == T_DIR);
}


void find(const char *path, const char *file_name)
{
  if (!is_dir(path)) {
    fprintf(2, "invalid search path %s, please check!\n", path);
    return;
  }

  do_find(path, file_name);
}


void do_find(const char *path, const char *file_name)
{
  char *p, buf[512] = {0};
  int fd;
  struct dirent de;

  basename(path, buf);
  if (strcmp(buf, file_name) == 0) {
    printf("%s\n", path);
    return;
  }

  if (!is_dir(path)) {
    return;
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
    fprintf(2, "path %s too long\n", path);
    return;
  }

  memset(buf, 0, sizeof(buf));
  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  fd = open(path, 0);
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if ((strcmp(de.name, ".") == 0)
        || (strcmp(de.name, "..") == 0)
        || (strlen(de.name) == 0)) {
      continue;
    }

    memset(p, 0, sizeof(buf) - strlen(buf));
    strcpy(p, de.name);
    do_find(buf, file_name);
  }

  *p-- = 0;

  close(fd);
}


int main(int argc, char *argv[])
{
  if (argc < 3) {
    printf("Please specify search path and destination file name\n");
    exit(0);
  }

  find(argv[1], argv[2]);
  exit(0);
}
