#include "../include/add_builtins.h"
#include "../include/hashmap.h"
#include "../include/run_program.h"
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

  if (fgets(command, size, stdin) == NULL) {
    p.argv = NULL;
    return p;
  };
  command[strcspn(command, "\n")] = '\0';

  p.argv = malloc(128 * sizeof(char *));
  if (!p.argv)
    return p;

  char *token = strtok(command, " ");
  while (token && p.argc < 127) {
    p.argv[p.argc++] = token;
    token = strtok(NULL, " ");
  }
  p.argv[p.argc] = NULL;

  return p;
}

int main() {
  add_builtins();

  char raw_command[1024];

  while (1) {

    setbuf(stdout, NULL);

    printf("$ ");

    pc command = parse_command(raw_command, sizeof(raw_command));

    if (!command.argv) {
      printf("\n");
      break;
    }

    if (!command.argv[0])
      continue;

    shell_builtin *builtin = hashmap_get(command.argv[0]);
    if (builtin != NULL) {
      builtin->func(command.argc, command.argv);
      free(command.argv);
      continue;
    }

    if (run_program(command.argc, command.argv) == -1) {
      printf("%s: command not found\n", command.argv[0]);
    }
    free(command.argv);
  }

  return 0;
}
