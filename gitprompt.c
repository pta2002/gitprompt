// A little prompt that displays git status
// Just add it to your PS1, can be customized in config.h
// Better than a script as it avoids the overhead of calling other programs,
// making it much faster.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define error(X) fprintf(stderr, "gitprompt: " X "\n")

typedef struct gitstatus {
    char *branch_name;
    int change_count;
} gitstatus;

#include "config.h"

typedef enum mode {
    READING_HASH,
    READING_BRANCH,
    SEEKING_END,
    COUNTING_ENTRIES
} MODE;

int main() {
    int error;
    FILE *fp;

    // git status --porcelain=v1 -b formats the git status output in a nicely
    // machine-readable format. We use -b as well so that the branch is
    // included in the output, since for some reason it isn't by default.
    // By using -z, we get our output separated by '0' instead of '\n', which
    // is handy for parsing
    //
    // This command lets us get most of what we will ever need for a shell
    // prompt.
    //
    // In the future maybe try something more direct than popen. It adds ~4ms
    // that we wouldn't get if we just used stdin and piped in git status to
    // the program
    //
    // TODO call this directly instead of using popen and relying on a shell
    if ((fp = popen("git status --porcelain=v1 -b -z 2> /dev/null", "r")) == NULL) {
        error("failed to run git");
        return -2;
    }

    // Time to parse the output. It's actually fairly complex, but we only
    // care about the branch, which will always be on the first line (due to
    // specifying -b) and the _number_ of changes, not the changes themselves,
    // so we can just count all lines after the first.
    // See also: https://git-scm.com/docs/git-status#_output
    int currentchar;
    int branch_pos = 0;
    gitstatus *s;
    MODE mode = READING_HASH;

    if ((s = malloc(sizeof(gitstatus))) == NULL
            || (s->branch_name = malloc(sizeof(128))) == NULL) {
        error("failed to allocate memory");
        return -1;
    }

    while ((currentchar = fgetc(fp)) != EOF) {
        // Prepare for very bad parsing code.
        // The format is more or less this:
        //
        // ##local-head...remote-head
        // XY file (-> file2)
        //
        // Due to using -z, it's all separated by '\0' characters, so not very
        // difficult to parse.

        switch (mode) {
            case READING_HASH:
                // Just ignore it until it's not a # or a space, at that point
                // we switch to reading a branch

                if (currentchar != '#' && currentchar != ' ') {
                    mode = READING_BRANCH;
                } else
                    break; // Only break if we're not carrying on to read
            case READING_BRANCH:
                // Now we are finally reading the branch name
                // We can just wait until we find a character that isn't part
                // of the branch name
                if (currentchar && currentchar != '.' && currentchar != '\n'
                        && branch_pos < 127) {
                    s->branch_name[branch_pos] = currentchar;
                    branch_pos++;
                    break;
                } else {
                    s->branch_name[branch_pos] = 0;
                    mode = SEEKING_END;
                }
            case SEEKING_END:
                // At this point, we're just waiting to finish the master
                // branch line, so we just wait for a '\0' and change
                // mode as soon as we do, so we begin counting the entries
                if (currentchar == 0)
                    mode = COUNTING_ENTRIES;
                break;
            case COUNTING_ENTRIES:
                if (currentchar == 0)
                    s->change_count++;
                break;
        }
    }

    int status = pclose(fp);

    if (status)
        return status;

    show_prompt(s);

    return 0;
}
