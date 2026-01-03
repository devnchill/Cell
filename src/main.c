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

void add_builtins(void) {
  shell_builtin e;
  e.command = "exit";
  e.info = "terminate the program with return code 0";
  e.func = builtin_exit;
  hashmap_add(e.command, e);
}

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  add_builtins();
  char command[1024];

  while (1) {

    printf("$ ");

    get_command(command, sizeof(command));

    shell_builtin *builtin = hashmap_get(command);
    if (builtin != NULL) {
      builtin->func(0, NULL);
      continue;
    }

    printf("%s: command not found\n", command);
  }

  return 0;
}
