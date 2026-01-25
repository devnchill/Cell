#include "../../include/parse_command.h"
#include <stdbool.h>
#include <stdio.h>

#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>

void flush_buffer_to_argv(ps *state, pc *cmd) {
  if (state->buffer_index == 0)
    return;
  state->buffer[state->buffer_index] = '\0';
  cmd->argv[cmd->argc++] = strdup(state->buffer);
  state->buffer_index = 0;
}

pc parse_command(const char *line) {
  pc cmd = {0};
  ps state = {0};

  state.line = line;

  cmd.argv = malloc(128 * sizeof(char *));
  if (!cmd.argv)
    return cmd;

  while (1) {
    char c = state.line[state.pos];

    if (c == '\\') {
      if (state.in_single_quotes) {
        state.buffer[state.buffer_index++] = '\\';
      } else if (state.in_double_quotes) {
        char next = line[state.pos + 1];
        if (next == '$' || next == '"' || next == '\\' || next == '\n') {
          state.pos++;
          state.buffer[state.buffer_index++] = next;
        } else {
          state.buffer[state.buffer_index++] = '\\';
        }
      } else {
        if (line[state.pos + 1] != '\0') {
          state.pos++;
          state.buffer[state.buffer_index++] = line[state.pos];
        }
      }
      continue;
    }

    if (!state.in_single_quotes && !state.in_double_quotes && c == '1' &&
        line[state.pos + 1] == '>') {

      flush_buffer_to_argv(&state, &cmd);

      state.pos += 2;

      while (line[state.pos] == ' ')
        state.pos++;

      char filebuf[1024];
      int fi = 0;
      while (line[state.pos] && line[state.pos] != ' ' &&
             fi < (int)sizeof(filebuf) - 1) {
        filebuf[fi++] = line[state.pos++];
      }
      filebuf[fi] = '\0';

      cmd.redirs.stdout_file = strdup(filebuf);
      state.pos--;
      continue;
    }
    if (!state.in_single_quotes && !state.in_double_quotes && c == '>') {
      flush_buffer_to_argv(&state, &cmd);

      state.pos++;
      while (line[state.pos] == ' ')
        state.pos++;

      char filebuf[1024];
      int fi = 0;
      while (line[state.pos] && line[state.pos] != ' ') {
        filebuf[fi++] = line[state.pos++];
      }
      filebuf[fi] = '\0';

      cmd.redirs.stdout_file = strdup(filebuf);
      state.pos--;
      continue;
    }
    if (c == '\'' && !state.in_double_quotes) {
      state.in_single_quotes = !state.in_single_quotes;
      continue;
    }

    if (c == '"' && !state.in_single_quotes) {
      state.in_double_quotes = !state.in_double_quotes;
      continue;
    }
    if ((c == ' ' || c == '\0') && !state.in_single_quotes &&
        !state.in_double_quotes) {
      flush_buffer_to_argv(&state, &cmd);
      if (c == '\0')
        break;
      continue;
    }
    state.buffer[state.buffer_index++] = c;
    state.pos++;
  }

  cmd.argv[cmd.argc] = NULL;
  return cmd;
}
