#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int builtin_pwd(int argc, char **argv) {
  char *path = malloc(PATH_MAX);
  if (!path) {
    perror("malloc in getcwd failed");
  }
  if (getcwd(path, PATH_MAX) == NULL) {
    perror("failed calling getcwd");
    free(path);
  } else {
    printf("%s\n", path);
  }
  free(path);
  return 0;
};
