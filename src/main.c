#include "../include/hashmap.h"
#include "../include/shell_builtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: will probably create a hashmap of type shell_builtin.command ->
// shell_builtin;

void get_command(char *command, size_t size) {
  fgets(command, size, stdin);
  command[strcspn(command, "\n")] = '\0';
  return;
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
  shell_builtin *builtin = hashmap_get(argv[1]);
  if (builtin != NULL) {
    printf("%s is a shell builtin\n", cmd);
  } else {
    printf("%s: not found\n", cmd);
  }
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

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  add_builtins();
  char command[1024];

  while (1) {

    printf("$ ");

    get_command(command, sizeof(command));

    if (command[0] == '\0')
      continue;

    char *argv[128];
    int argc = 0;

    char *token = strtok(command, " ");
    while (token != NULL && argc < 128) {
      argv[argc++] = token;
      token = strtok(NULL, " ");
    }

    shell_builtin *builtin = hashmap_get(argv[0]);
    if (builtin != NULL) {
      builtin->func(argc, argv);
      continue;
    }

    printf("%s: command not found\n", command);
  }

  return 0;
}
