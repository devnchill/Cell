#include <stdio.h>

#include <readline/readline.h>

char *command_generator(const char *text, int state) {
  static int index;
  static const char *commands[] = {"echo", "exit", NULL};
  if (state == 0)
    index = 0;

  while (commands[index]) {
    const char *cmd = commands[index++];
    if (strncmp(cmd, text, strlen(text)) == 0)
      return strdup(cmd);
  }
  return NULL;
}

char **command_completion(const char *text, int start, int end) {
  (void)start;
  (void)end;
  return rl_completion_matches(text, command_generator);
}

void init_auto_completion() {
  rl_attempted_completion_function = command_completion;
  rl_complete(0, '\t');
}
