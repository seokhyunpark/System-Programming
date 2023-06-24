#include <stdio.h>
#include "smsh.h"

int is_control_command(char *);
int do_control_command(char **);
int ok_to_execute();
int builtin_command(char **args, int *resultp);

int process(char **args) {
    int rv = 0;
    if (args[0] == NULL) rv = 0;
    else if (builtin_command(args, &rv));
    else if (ok_to_execute())
        rv = execute(args);
    return rv;
}
