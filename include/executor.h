#ifndef EXECUTOR_H
#define EXECUTOR_H

/* ---- Part 4: $PATH Search (Gabriel + Olivia) --------------------------
 * Returns a newly heap-allocated absolute/relative path to the first
 * matching executable for `cmd`, or NULL if none was found (caller
 * should then print a "command not found"-style error -- this function
 * should NOT print anything itself, so it stays reusable inside a
 * pipeline where the caller controls how/when errors are reported).
 *
 * Rules from the handout + TA slides:
 *  - if `cmd` contains a '/', treat it as a literal path: check with
 *    access(cmd, X_OK) and return a copy of `cmd` (or NULL).
 *  - otherwise, search a private copy of PATH in directory order.
 *    Empty entries mean the current directory; an unset PATH has no matches.
 *  - matches must be regular files with execute permission.
 * Returns NULL for an empty command or allocation failure as well.
 * The environment's PATH is never modified. */
char *find_executable(const char *cmd);

/* ---- Part 5: External Command Execution (Olivia + Gannon) -------------
 * Runs one external command: argv[0] must already be the resolved path
 * from find_executable(), argv must be NULL-terminated, and the array
 * itself must include the resolved path as argv[0] (execv expects this).
 *
 * Must fork() first. In the child: call execv(argv[0], argv). If execv()
 * returns at all, it failed -- print an error and call _exit() (NOT
 * exit(), to avoid flushing/duplicating the parent's stdio buffers and
 * running atexit handlers twice). In the parent: waitpid() on the child
 * UNLESS this command is meant to run in the background (Part 8 decides
 * that -- this function should take a flag or be called differently for
 * background jobs; work that integration out with whoever owns
 * background.c).
 * Background execution is not yet implemented. */
int run_external(char **argv);

/* Foreground execution with optional redirection applied only in the child. */
int run_external_redirected(char **argv, const char *infile, const char *outfile);

#endif
