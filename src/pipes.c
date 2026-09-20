#include <stdio.h>
#include "pipes.h"

/* ---- Part 7: Gabriel + Gannon ------------------------------------------
 * Stubbed so the rest of the project keeps compiling. See pipes.h for
 * the fd-wiring contract (n-1 pipes, n forks, which fds to dup2/close in
 * which child) and design-notes.md for the full walkthrough -- the TA
 * slides specifically warn that forgetting to close the unused pipe
 * ends is the most common bug here. */

int run_pipeline(cmd_t *cmds, int n, int background) {
    (void)cmds;
    (void)n;
    (void)background;
    /* TODO (Part 7): pipe()/fork() n-1 / n times, wire fds, call
     * apply_redirection() then run_external() (or an execv-based
     * variant) in each child, then waitpid() in the parent unless
     * background is set (hand off to background.c in that case). */
    fprintf(stderr, "run_pipeline: not implemented yet\n");
    return -1;
}
