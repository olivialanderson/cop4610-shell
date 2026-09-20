#include <stdio.h>
#include "executor.h"

/* ---- Part 4/5: Gabriel + Olivia (PATH search) / Olivia + Gannon (exec) -
 * Stubbed for now so the rest of the project keeps compiling. See
 * executor.h for the exact contract and design-notes.md in the course
 * project for the fork/execv walkthrough and the deep-copy-PATH warning
 * from the TA slides. */

char *find_executable(const char *cmd) {
    (void)cmd;
    /* TODO (Part 4): handle the cmd-contains-'/' case, then the PATH
     * search case (deep copy getenv("PATH"), strtok on ':', access()
     * each candidate). Return NULL (no match) for now. */
    return NULL;
}

int run_external(char **argv) {
    (void)argv;
    /* TODO (Part 5): fork(), execv() in the child, waitpid() in the
     * parent (unless backgrounded -- coordinate with background.c). */
    fprintf(stderr, "run_external: not implemented yet\n");
    return -1;
}
