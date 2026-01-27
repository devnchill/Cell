#include "../../include/parser/parser.h"

void handle_escape_sequence(ps *state) {
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
