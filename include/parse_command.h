#ifndef PARSE_COMMAND
#define PARSE_COMMAND

#include <stddef.h>
typedef struct parsed_command {
  int argc;
  char **argv;
} pc;

pc parse_command(char *command, size_t size);

#endif
