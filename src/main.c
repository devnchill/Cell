#include "../include/hashmap.h"
#include "../include/shell_builtin.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct parsed_command {
  int argc;
  char **argv;
} pc;

pc parse_command(char *command, size_t size) {
  pc p = {0};

  fgets(command, size, stdin);
  command[strcspn(command, "\n")] = '\0';

  p.argv = malloc(128 * sizeof(char *));
  if (!p.argv)
    return p;

  char *token = strtok(command, " ");
  while (token && p.argc < 128) {
    p.argv[p.argc++] = token;
    token = strtok(NULL, " ");
  }

  return p;
}

int builtin_exit(int argc, char **argv) { exit(0); }
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
int builtin_type(int argc, char **argv) {
  char *cmd = argv[1];
  shell_builtin *builtin = hashmap_get(cmd);
  if (builtin != NULL) {
    printf("%s is a shell builtin\n", cmd);
    return 0;
  }
  char *path = getenv("PATH");
  if (path == NULL) {
    printf("path not found\n");
    return -1;
  }
  char *path_copy = strdup(path);
  char *token = strtok(path_copy, ":");
  while (token) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", token, cmd);
    if (access(full_path, X_OK) == 0) {
      printf("%s is %s\n", cmd, full_path);
      free(path_copy);
      return 0;
    }
    token = strtok(NULL, ":");
  }
  free(path_copy);
  printf("%s: not found\n", cmd);
  return 0;
}

void add_builtins(void) {
  // exit
  shell_builtin exit;
  exit.command = "exit";
  exit.info = "terminate the program with return code 0";
  exit.func = builtin_exit;
  hashmap_add(exit.command, exit);

  shell_builtin echo;
  echo.command = "echo";
  echo.info = "print everything after echo";
  echo.func = builtin_echo;
  hashmap_add(echo.command, echo);

  shell_builtin type;
  type.command = "type";
  type.info = "check whether the type of command is builtin or not";
  type.func = builtin_type;
  hashmap_add(type.command, type);
}

int main() {
  // Flush after every printf
  setbuf(stdout, NULL);

  add_builtins();
  char raw_command[1024];

  while (1) {

    printf("$ ");

    pc command = parse_command(raw_command, sizeof(raw_command));

    if (command.argv[0] == NULL)
      continue;

    shell_builtin *builtin = hashmap_get(command.argv[0]);
    if (builtin != NULL) {
      builtin->func(command.argc, command.argv);
      continue;
    }

    printf("%s: command not found\n", command.argv[0]);
  }

  return 0;
}
