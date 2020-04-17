#ifndef __GITPROMPT_H_
#define __GITPROMPT_H_

#define error(X) fprintf(stderr, "gitprompt: " X "\n")

typedef struct gitstatus {
    char *branch_name;
    int change_count;
} gitstatus;

void print_status(gitstatus *s);

#endif // __GITPROMPT_H_
