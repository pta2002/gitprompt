// A little prompt that displays git status
// Just add it to your PS1, can be customized in config.h
// Better than a script as it avoids the overhead of calling other programs,
// making it much faster.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>

static const char *branch_name;  // Name of the current branch
static git_status_list *status;
static int change_num;           // Amount of changes

#include "config.h"

const char *get_current_branch_name(git_repository*);

int main() {
    git_repository* repo; // The repository
    int error;

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

#ifdef HAVE_BRANCH_STATUS
    git_status_options opts;
    git_status_options_init(&opts, 1);
    if (error = git_status_list_new(&status, repo, &opts))
        return error;
    change_num = git_status_list_entrycount(status);
#endif

#ifdef HAVE_BRANCH_NAME
    // I have to put this after getting the branch status or it will not show the branch
    // name on some repos (at least for the linux kernel repo I tested this on
    branch_name = get_current_branch_name(repo);
#endif

    print_status();

    return 0;
}

#ifdef HAVE_BRANCH_NAME
// Get the name of the current branch.
// libgit2 seems to have issues with getting the name of the current branch if
// there are no commits, so in that case for now we're just defaulting to "master"
// (which will likely be the case anyway), in the future we can either fix this
// or even manually parse the HEAD file, which would tell us the name of the branch.
// (or maybe just grep `git status`, which does tell you the current branch, though
// that's less ideal).
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
#endif
