#ifndef BUILTINS_H
#define BUILTINS_H

/* ---- Part 9: Internal Command Execution (Gabriel + Olivia) -----------
 * Builtins run directly in the shell's own process -- never via
 * fork()+execv(). Each returns 1 if `argv[0]` was a recognized builtin
 * (and it handled the command), 0 otherwise (so main.c knows to fall
 * through to external-command handling). */

/* exit: waits for all background jobs (see background.h), prints the
 * last up-to-3 valid commands (fewer if fewer exist, or a "no valid
 * commands" message), frees owned memory, then exits the process.
 * NOT YET IMPLEMENTED. */
int builtin_exit(char **argv);

/* cd: changes directory via chdir(). No args -> chdir to getenv("HOME").
 * More than one arg, target doesn't exist, or target isn't a directory
 * are each distinct error cases the handout calls out explicitly.
 * NOT YET IMPLEMENTED. */
int builtin_cd(char **argv);

/* jobs: prints active background jobs via print_jobs() (background.h).
 * NOT YET IMPLEMENTED. */
int builtin_jobs(char **argv);

/* Records `cmdline` into a ring buffer of the last 3 "valid" commands,
 * for builtin_exit() to print. Call this once per successfully-parsed
 * (not necessarily successfully-executed) command in main.c's loop.
 * NOT YET IMPLEMENTED. */
void record_valid_command(const char *cmdline);

#endif
