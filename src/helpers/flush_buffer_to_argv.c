#include "../../include/parser/parser.h"
#include <string.h>

void flush_buffer_to_argv(ps *state, pc *cmd) {
  if (state->buffer_index == 0)
    return;
  state->buffer[state->buffer_index] = '\0';
  cmd->argv[cmd->argc++] = strdup(state->buffer);
  state->buffer_index = 0;
}
