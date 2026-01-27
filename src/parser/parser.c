#include "../../include/parser/parser.h"
#include "../../include/helpers/flush_buffer_to_argv.h"
#include "../../include/parser/handle_escape.h"
#include "../../include/parser/stderr.h"
#include "../../include/parser/stdout.h"
#include <stdbool.h>
#include <stdio.h>

#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>

void append(pipeline_t *pl, pc *cmd) {
  pl->commands = realloc(pl->commands, sizeof(pc) * (pl->count + 1));

  if (!pl->commands) {
    perror("realloc");
    exit(1);
  }

  pl->commands[pl->count] = *cmd;
  pl->count++;
}

pipeline_t parse_command(const char *line) {
  pipeline_t pl = {0};
  pc curr_cmd = {0};
  ps state = {0};

  state.line = line;

  curr_cmd.argv = malloc(128 * sizeof(char *));
  if (!curr_cmd.argv)
    return pl;

  while (1) {
    char c = state.line[state.pos];

    if (c == '|') {
      flush_buffer_to_argv(&state, &curr_cmd);
      curr_cmd.argv[curr_cmd.argc] = NULL;
      append(&pl, &curr_cmd);
      memset(&curr_cmd, 0, sizeof(pc));
      curr_cmd.argv = malloc(128 * sizeof(char *));
      state.pos++;
      state.buffer_index = 0;
      continue;
    }

    // escape
    if (c == '\\') {
      handle_escape_sequence(&state);
      continue;
    }

    if (!state.in_single_quotes && !state.in_double_quotes && c == '2' &&
        state.line[state.pos + 1] == '>' && state.line[state.pos + 2] == '>') {
      curr_cmd.redirs.stderr_append = true;
      parse_stderr(&curr_cmd, &state, 3);
      continue;
    }

    if (!state.in_single_quotes && !state.in_double_quotes && c == '1' &&
        state.line[state.pos + 1] == '>' && state.line[state.pos + 2] == '>') {
      curr_cmd.redirs.stdout_append = true;
      parse_stdout(&curr_cmd, &state, 3);
      continue;
    } else if (!state.in_single_quotes && !state.in_double_quotes && c == '>' &&
               state.line[state.pos + 1] == '>') {
      curr_cmd.redirs.stdout_append = true;
      parse_stdout(&curr_cmd, &state, 2);
      continue;
    }

    if (!state.in_single_quotes && !state.in_double_quotes && c == '2' &&
        state.line[state.pos + 1] == '>') {
      parse_stderr(&curr_cmd, &state, 2);
      continue;
    }

    if (!state.in_single_quotes && !state.in_double_quotes && c == '1' &&
        state.line[state.pos + 1] == '>') {
      parse_stdout(&curr_cmd, &state, 2);
      continue;
    } else if (!state.in_single_quotes && !state.in_double_quotes && c == '>') {
      parse_stdout(&curr_cmd, &state, 1);
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
      flush_buffer_to_argv(&state, &curr_cmd);
      if (c == '\0')
        break;
      state.pos++;
      continue;
    }
    state.buffer[state.buffer_index++] = c;
    state.pos++;
  }
  flush_buffer_to_argv(&state, &curr_cmd);
  curr_cmd.argv[curr_cmd.argc] = NULL;
  if (curr_cmd.argc > 0 || curr_cmd.redirs.stdout_file ||
      curr_cmd.redirs.stderr_file) {
    append(&pl, &curr_cmd);
  }
  return pl;
}
