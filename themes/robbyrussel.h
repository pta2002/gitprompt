#ifndef GP_THEME_H
#define GP_THEME_H

#include <stdio.h>
#include "colors.h"

#define HAVE_BRANCH_NAME    // exposes variable `branch_name` with the branch name
#define HAVE_BRANCH_STATUS  // exposes variable `change_num`

// This is where the output will go, it's just some code that'll get called.
void show_prompt(gitstatus *s) {
    printf(FG_BLUE "git:(" FG_RED "%s" FG_BLUE ")" FG_DEFAULT, s->branch_name);
    if (s->change_count) printf(FG_YELLOW " ✗%d" FG_DEFAULT, s->change_count);
}

/* #define PRINT_STATUS \ */
/*     printf(FG_BLUE "git:(" FG_RED "%s" FG_BLUE ")" FG_DEFAULT, branch); \ */
/*     if (dirty) printf(FG_YELLOW "✗" FG_DEFAULT); */

#endif
