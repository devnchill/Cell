#include "../../include/parser/parser.h"
#include "../../include/parser/stderr.h"
#include "../../include/parser/stdout.h"
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

static void handle_escape_sequence(ps *state) {
  char next = state->line[state->pos + 1];

  // just add it as it is
  if (state->in_single_quotes) {
    state->buffer[state->buffer_index++] = '\\';
    state->pos++;
    return;
  }

  if (state->in_double_quotes) {
    if (next == '$' || next == '"' || next == '\\' || next == '\n') {
      state->buffer[state->buffer_index++] = next;
      state->pos += 2;
    } else {
      state->buffer[state->buffer_index++] = '\\';
      state->pos++;
    }
    return;
  }

  if (next != '\0') {
    state->buffer[state->buffer_index++] = next;
    state->pos += 2;
  } else {
    state->pos++;
  }
}

void free_command(pc *cmd) {
  if (!cmd)
    return;
  if (cmd->argv) {
    for (int i = 0; i < cmd->argc; i++)
      free(cmd->argv[i]);
    free(cmd->argv);
  }
  if (cmd->redirs.stdout_file)
    free(cmd->redirs.stdout_file);
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

    // escape
    if (c == '\\') {
      handle_escape_sequence(&state);
      continue;
    }

    if (!state.in_single_quotes && !state.in_double_quotes && c == '2' &&
        state.line[state.pos + 1] == '>') {
      parse_stderr(&cmd, &state);
      continue;
    }

    if (!state.in_single_quotes && !state.in_double_quotes && c == '1' &&
        state.line[state.pos + 1] == '>') {
      parse_stdout(&cmd, &state, 2);
      continue;
    } else if (!state.in_single_quotes && !state.in_double_quotes && c == '>') {
      parse_stdout(&cmd, &state, 1);
      continue;
    }

    if (c == '"' && !state.in_single_quotes) {
      state.in_double_quotes = !state.in_double_quotes;
      state.pos++;
      continue;
    }

    if (c == '\'' && !state.in_double_quotes) {
      state.in_single_quotes = !state.in_single_quotes;
      state.pos++;
      continue;
    }

    if ((c == ' ' || c == '\0') && !state.in_single_quotes &&
        !state.in_double_quotes) {
      flush_buffer_to_argv(&state, &cmd);
      if (c == '\0')
        break;
      state.pos++;
      continue;
    }
    state.buffer[state.buffer_index++] = c;
    state.pos++;
  }

  cmd.argv[cmd.argc] = NULL;
  return cmd;
}
