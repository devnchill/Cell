#include "../../include/helpers/free_pipeline.h"

void free_pipeline(pipeline_t *pl) {
  if (!pl)
    return;

  for (size_t i = 0; i < pl->count; i++) {
    pc *cmd = &pl->commands[i];
    for (size_t j = 0; j < cmd->argc; j++)
      free(cmd->argv[j]);
    free(cmd->argv);

    if (cmd->redirs.stdout_file)
      free(cmd->redirs.stdout_file);
    if (cmd->redirs.stderr_file)
      free(cmd->redirs.stderr_file);
  }

  free(pl->commands);
  pl->commands = NULL;
  pl->count = 0;
}
