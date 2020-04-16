#ifndef GP_THEME_H
#define GP_THEME_H

#include <stdio.h>
#include "colors.h"

#define HAVE_BRANCH_NAME    // exposes variable `branch_name` with the branch name
#define HAVE_BRANCH_STATUS  // exposes variable `change_num`

// This is where the output will go, it's just some code that'll get called.
void show_prompt(gitstatus *s) {
    char *change = "";

    if (s->change_count) change = FG_YELLOW "*";

    printf(FG_BLUE "(" FG_LGREEN "%s%s" FG_BLUE ")" FG_DEFAULT,
           s->branch_name, change);
}

#endif
