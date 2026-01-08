#include "../include/add_builtins.h"
#include "../include/hashmap.h"
#include "../include/init_auto_completion.h"
#include "../include/run_program.h"
#include "../include/shell_builtin.h"
#include "../include/tokenize_command.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <readline/readline.h>

int main() {
  setbuf(stdout, NULL);

  init_auto_completion();
  add_builtins();

  while (1) {

    tc command = tokenize_command();

    if (!command.argv) {
      printf("\n");
      break;
    }

    if (!command.argv[0])
      continue;

    shell_builtin *builtin = hashmap_get(command.argv[0]);
    if (builtin) {
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
