#include "../include/builtins/add_builtins.h"
#include "../include/builtins/shell_builtin.h"
#include "../include/helpers/hashmap.h"
#include "../include/init_auto_completion.h"
#include "../include/load_history.h"
#include "../include/parser/parser.h"
#include "../include/redirect/restore_fd.h"
#include "../include/redirect/stderr.h"
#include "../include/redirect/stdout.h"
#include "../include/run_program.h"
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>

#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>

#include <readline/readline.h>

int main() {
  setbuf(stdout, NULL);

  init_auto_completion();
  add_builtins();
  load_history();

  while (1) {

    char *line = readline("$ ");
    if (!line)
      break;

    add_history(line);

    pc command = parse_command(line);

    free(line);

    if (!command.argv || !command.argv[0]) {
      printf("\n");
      free_command(&command);
      continue;
    }

    shell_builtin *builtin = hashmap_get(command.argv[0]);

    if (builtin) {
      int saved_stdout = -1, saved_stderr = -1;
      if (command.redirs.stderr_file) {
        saved_stderr = dup(STDERR_FILENO);
        redirect_stderr(&command);
      }

      if (command.redirs.stdout_file) {
        saved_stdout = dup(STDOUT_FILENO);
        redirect_stdout(&command);
      }

      builtin->func(command.argc, command.argv);

      restore_fd(saved_stdout, saved_stderr);
      free_command(&command);
      continue;
    }

    int status = run_program(&command);
    if (status == 127) {
      printf("%s: command not found\n", command.argv[0]);
    }
    free_command(&command);
  }

  return 0;
}
