#include <stdio.h>
int builtin_echo(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    printf("%s", argv[i]);
    if (i < argc - 1)
      printf(" ");
    else
      printf("\n");
  }
  return 0;
}
