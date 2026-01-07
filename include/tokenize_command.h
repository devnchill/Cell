#ifndef PARSE_COMMAND
#define PARSE_COMMAND

#include <stddef.h>
typedef struct tokenized_command {
  size_t argc;
  char **argv;
} tc;

tc tokenize_command();

#endif
