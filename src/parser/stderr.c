#include "../../include/parser/stderr.h"
#include "../../include/parser/parser.h"
#include <string.h>

void parse_stderr(pc *cmd, ps *state, int inc) {
  flush_buffer_to_argv(state, cmd);

  state->pos += inc;

  while (state->line[state->pos] == ' ')
    state->pos++;

  char filebuf[1024];
  int fi = 0;
  while (state->line[state->pos] && state->line[state->pos] != ' ' &&
         fi < (int)sizeof(filebuf) - 1) {
    filebuf[fi++] = state->line[state->pos++];
  }
  filebuf[fi] = '\0';

  cmd->redirs.stderr_file = strdup(filebuf);
}
