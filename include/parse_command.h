#ifndef PARSE_COMMAND
#define PARSE_COMMAND
#include <stdbool.h>
#include <stddef.h>

typedef struct redirections {
  char *stdout_file;
  char *stderr_file;
  bool stdout_append;
  bool stderr_append;
} redirections_t;

typedef struct parsed_command {
  size_t argc;
  char **argv;
  redirections_t redirs;
} pc;

typedef struct parser_state {
  bool in_single_quotes;
  bool in_double_quotes;
  const char *line;
  char buffer[1024];
  int buffer_index;
  size_t pos;
  pc *cmd;
} ps;

pc parse_command(const char *line);

#endif
