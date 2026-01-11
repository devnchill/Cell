#include "../include/add_builtins.h"
#include "../include/hashmap.h"
#include "../include/init_auto_completion.h"
#include "../include/load_history.h"
#include "../include/run_program.h"
#include "../include/shell_builtin.h"
#include "../include/tokenize_command.h"
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <readline/readline.h>

int main() {
  setbuf(stdout, NULL);

  init_auto_completion();
  add_builtins();
  load_history();

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
      int saved_stdout = -1;
      if (command.stdout_file) {
        int fd = open(command.stdout_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
          perror("open");
          free(command.argv);
          continue;
        }
        saved_stdout = dup(STDOUT_FILENO);
        dup2(fd, STDOUT_FILENO);
        close(fd);
      }
      builtin->func(command.argc, command.argv);
      if (saved_stdout != -1) {
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
      }
      free(command.argv);
      continue;
    }

    if (run_program(&command) == -1) {
      printf("%s: command not found\n", command.argv[0]);
    }
    free(command.argv);
  }

  return 0;
}
