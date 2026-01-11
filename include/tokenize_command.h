#ifndef PARSE_COMMAND
#define PARSE_COMMAND

#include <stddef.h>
typedef struct tokenized_command {
  size_t argc;
  char **argv;
  char *stdout_file;
  int stdout_append;
} tc;

tc tokenize_command();

#endif
