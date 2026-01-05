#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int builtin_cd(int argc, char **argv) {
  if (argc == 1 || strcmp(argv[1], "~") == 0) {
    int ret = chdir(getenv("HOME"));
    if (ret == -1) {
      perror("error when trying to cd home");
    }
    return 0;
  }
  if (argc < 2) {
    perror("expected path for cd,recieved none");
  }
  char *path = argv[1];
  int ret = chdir(path);
  if (ret != 0) {
    printf("cd: %s: No such file or directory\n", path);
    return ret;
  }
  return 0;
}
