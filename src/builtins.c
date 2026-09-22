#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "builtins.h"

/* ---- Part 9: Gabriel + Olivia ------------------------------------------
 * Stubbed so the rest of the project keeps compiling. See builtins.h for
 * the exact contract for each builtin and design-notes.md for the
 * exit/cd/jobs error cases called out in the handout. Remember: none of
 * these may call execv() -- they run directly in the shell's process. */

int builtin_exit(char **argv) {
    (void)argv;
    /* TODO (Part 9): call wait_all_background_jobs() (background.h),
     * print the last up-to-3 valid commands, free owned memory, then
     * exit(0). Returning from here (rather than exiting) is a bug --
     * main.c currently handles "exit" itself as a placeholder; once this
     * is implemented, move that responsibility here. */
    return 1;
}

int builtin_cd(char **argv) {
    char *target;

    if (argv[1] == NULL) {
        target = getenv("HOME");
    } else if (argv[2] != NULL) {
        fprintf(stderr, "cd: too many arguments\n");
        return 1;
    } else {
        target = argv[1];
    }

    if (chdir(target) != 0) {
        perror("cd");
    }

    return 1;
}

int builtin_jobs(char **argv) {
    (void)argv;
    /* TODO (Part 9): call print_jobs() (background.h) once it's real. */
    fprintf(stderr, "jobs: not implemented yet\n");
    return 1;
}

void record_valid_command(const char *cmdline) {
    (void)cmdline;
    /* TODO (Part 9, feeds `exit`): keep a ring buffer of the last 3
     * valid command lines (a fixed array of 3 char* plus a count is
     * enough -- you don't need anything fancier). */
}
