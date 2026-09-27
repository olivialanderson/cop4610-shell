#ifndef BUILTINS_H
#define BUILTINS_H
/* Builtins return 0 on success and 1 on error. No builtin calls execv. */
int is_builtin(const char *name);
int builtin_cd(char **argv);
int builtin_jobs(char **argv);
/* Finish history and wait for jobs; the caller then leaves its main loop. */
int builtin_exit(char **argv);
void record_valid_command(const char *cmdline);
#endif
