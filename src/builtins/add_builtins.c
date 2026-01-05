#include "../../include/builtin_cd.h"
#include "../../include/builtin_echo.h"
#include "../../include/builtin_exit.h"
#include "../../include/builtin_pwd.h"
#include "../../include/builtin_type.h"
#include "../../include/hashmap.h"
#include "../../include/shell_builtin.h"

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
}
