#include <stdio.h>
#include "background.h"

/* ---- Part 8: Gabriel + Olivia ------------------------------------------
 * Stubbed so the rest of the project keeps compiling. See background.h
 * for what each function must do (the job table, WNOHANG polling, the
 * "[n] pid" / "[n]+ done ..." message formats) and design-notes.md for
 * the suggested job_t layout. */

void add_background_job(pid_t pid, const char *cmdline) {
    (void)pid;
    (void)cmdline;
    /* TODO (Part 8): store into a static job_t table (size MAX_BG_JOBS),
     * assign the next job number, print "[n] <pid>". */
}

void check_background_jobs(void) {
    /* TODO (Part 8): waitpid(pid, &status, WNOHANG) each tracked job;
     * on exit, print "[n]+ done <cmdline>" and free/clear that slot. */
}

void print_jobs(void) {
    /* TODO (Part 9 builtin `jobs` depends on this): print each active
     * job as "[n]+ <pid> <cmdline>", or a message if none are active. */
    printf("no active background jobs\n"); /* placeholder until the job table exists */
}

void wait_all_background_jobs(void) {
    /* TODO (Part 9 builtin `exit` depends on this): blocking waitpid()
     * on every still-active job, then free the table's memory. */
}
