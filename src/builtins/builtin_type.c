#include "../../include/builtins/shell_builtin.h"
#include "../../include/helpers/hashmap.h"
#include "../../include/helpers/is_present_in_path.h"
#include <stdio.h>
#include <unistd.h>

int builtin_type(int argc, char **argv) {
  char *cmd = argv[1];
  shell_builtin *builtin = hashmap_get(cmd);
  if (builtin != NULL) {
    printf("%s is a shell builtin\n", cmd);
    return 0;
  }
  executable_in_system_path eisp = executable_is_present(cmd);
  if (eisp.is_present) {
    printf("%s is %s\n", cmd, eisp.full_path);
  } else {
    printf("%s: not found\n", cmd);
  }
  return 0;
}
