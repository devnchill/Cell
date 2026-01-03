#ifndef SHELL_BUILTIN_H
#define SHELL_BUILTIN_H

typedef struct shell_builtin {
  char *info;
  int (*func)(int argc, char **argv);
  char *command;
} shell_builtin;

#endif
