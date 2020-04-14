// A little prompt that displays git status
// Just add it to your PS1, can be customized in config.h
// Better than a script as it avoids the overhead of calling other programs,
// making it much faster.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>
#include "config.h"

const char *get_current_branch_name(git_repository*);

int main() {
    git_repository* repo; // The repository

    // Initialize libgit2
    // This is used to get all git status necessary
    git_libgit2_init();

    // Detect if we are inside a git repo. If we are, populate the repo variable.
    if (git_repository_open_ext(&repo, NULL, GIT_REPOSITORY_OPEN_FROM_ENV, NULL)) {
        // We are not inside a git repo.
        // This will probably happen most of the time, since it'll be run as part of
        // the prompt, just return 1 and don't waste any more valuable time.
        return 1;
    }

    const char *branch = get_current_branch_name(repo);
    printf("%s\n", branch);

    return 0;
}

// Get the name of the current branch.
const char *get_current_branch_name(git_repository *repo) {
    git_reference *head;
    const char *name;
    int error;

    error = git_repository_head(&head, repo);

#ifdef UNBORN_SHOW
    // Get the repository name
    if (error == GIT_EUNBORNBRANCH)
        return UNBORN_REPO_NAME;
#endif
    // The repository isn't unborn; so if there's an error just return.
    // In this case, we don't want to do anything in case the repository is unborn,
    // so just return.
    if (error)
        return NULL;

    name = git_reference_shorthand(head);
    git_reference_free(head);

    return name;
}
