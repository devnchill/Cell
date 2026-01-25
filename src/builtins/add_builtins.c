#include "../../include/builtins/builtin_cd.h"
#include "../../include/builtins/builtin_echo.h"
#include "../../include/builtins/builtin_exit.h"
#include "../../include/builtins/builtin_history.h"
#include "../../include/builtins/builtin_pwd.h"
#include "../../include/builtins/builtin_type.h"
#include "../../include/builtins/shell_builtin.h"
#include "../../include/helpers/hashmap.h"

void add_builtins(void) {
  static shell_builtin exit = {.command = "exit",
                               .info =
                                   "terminate the program with return code 0",
                               .func = builtin_exit};
  hashmap_add(exit.command, exit);

  static shell_builtin echo = {.command = "echo",
                               .info = "print everything after echo",
                               .func = builtin_echo};
  hashmap_add(echo.command, echo);

  static shell_builtin type = {
      .command = "type",
      .info = "check whether the type of command is builtin or not",
      .func = builtin_type};
  hashmap_add(type.command, type);

  static shell_builtin pwd = {.command = "pwd",
                              .info = "return current working directory",
                              .func = builtin_pwd};
  hashmap_add(pwd.command, pwd);

  static shell_builtin cd = {.command = "cd",
                             .info = "change current working directory",
                             .func = builtin_cd};
  hashmap_add(cd.command, cd);

  static shell_builtin history = {.command = "history",
                                  .info = "change current working directory",
                                  .func = builtin_history};
  hashmap_add(history.command, history);
}
