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
 *  - else, deep-copy getenv("PATH") FIRST (never strtok the real env
 *    string -- that corrupts the process's actual $PATH), strtok() it on
 *    ':', and for each directory build "dir/cmd" and access(..., X_OK)
 *    it. Return a copy of the first match.
 * NOT YET IMPLEMENTED. */
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
 * NOT YET IMPLEMENTED. */
int run_external(char **argv);

#endif
