// A little prompt that displays git status
// Just add it to your PS1, can be customized in config.h
// Better than a script as it avoids the overhead of calling other programs,
// making it much faster.

#include "gitprompt.h"
#include "config.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef enum mode {
  READING_HASH,
  READING_BRANCH,
  SEEKING_END,
  COUNTING_ENTRIES
} MODE;

int main() {
  // git status --porcelain=v1 -b formats the git status output in a nicely
  // machine-readable format. We use -b as well so that the branch is
  // included in the output, since for some reason it isn't by default.
  // By using -z, we get our output separated by '0' instead of '\n', which
  // is handy for parsing
  //
  // This command lets us get most of what we will ever need for a shell
  // prompt.
  //
  // To improve performance, we handle the execution manually, instead of
  // relying on popen. This saves around 10ms and halves execution time, so
  // it's definitely worth the extra code!
  int pipes[2];
  int pid;
  pipe(pipes);
  if (!(pid = fork())) {
    int null = open("/dev/null", O_WRONLY);
    dup2(pipes[1], 1);
    close(pipes[0]);
    dup2(null, 2);
    execlp("git", "git", "status", "--porcelain=v1", "-b", "-z", NULL);
    _exit(0);
  }

  close(pipes[1]);

  // Time to parse the output. It's actually fairly complex, but we only
  // care about the branch, which will always be on the first line (due to
  // specifying -b) and the _number_ of changes, not the changes themselves,
  // so we can just count all lines after the first.
  // See also: https://git-scm.com/docs/git-status#_output
  int currentchar;
  int branch_pos = 0;
  gitstatus *s;
  MODE mode = READING_HASH;
  char buf[512];

  if ((s = malloc(sizeof(gitstatus))) == NULL ||
      (s->branch_name = malloc(128)) == NULL) {
    error("failed to allocate memory");
    return -1;
  }

  int bytes_read;

  while ((bytes_read = read(pipes[0], buf, 512)) != 0) {
    for (int i = 0; i < bytes_read; i++) {
      currentchar = buf[i];
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
        if (currentchar && currentchar != '.' && currentchar != '\n' &&
            branch_pos < 127) {
          // We only find a space if the repo has no commits
          // ("No commits yet on brach master"), so if we just
          // reset the branch name on a space we'll get just the
          // last word, which is the branch name.
          if (currentchar == ' ') {
            branch_pos = 0;
            break;
          }

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
  }

  close(pipes[0]);
  close(pipes[1]);

  // If there is an error with git (for example, we are not in a git
  // repository), exit the program.
  int status;
  waitpid(pid, &status, 0);
  if (status)
    return status;

  // Call the theme's show_prompt function, this will be be included via
  // config.h, since it'll include the theme file.
  show_prompt(s);

  return 0;
}
