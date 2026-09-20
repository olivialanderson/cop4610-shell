#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <sys/types.h>

#define MAX_BG_JOBS 10 /* handout guarantees no more than 10 concurrent */

typedef struct {
    int job_num;   /* starts at 1, never reused */
    pid_t pid;     /* last subcommand's pid, if it was a pipeline */
    char *cmdline; /* heap copy of the original command line, for the "done" message */
    int active;    /* 1 if this slot holds a live job */
} job_t;

/* ---- Part 8: Background Processing (Gabriel + Olivia) -----------------
 * Registers a newly started background job (prints "[n] <pid>" per the
 * handout) and stores it in an internal job table (static in
 * background.c, size MAX_BG_JOBS).
 * NOT YET IMPLEMENTED. */
void add_background_job(pid_t pid, const char *cmdline);

/* Called once per main-loop iteration, BEFORE printing the next prompt.
 * For every tracked job, waitpid(pid, &status, WNOHANG) (non-blocking!).
 * If a job has exited, print "[n]+ done <cmdline>" and remove it from
 * the table.
 * NOT YET IMPLEMENTED. */
void check_background_jobs(void);

/* Used by the `jobs` builtin: prints "[n]+ <pid> <cmdline>" for every
 * active job, or a "no active jobs" message if the table is empty.
 * NOT YET IMPLEMENTED. */
void print_jobs(void);

/* Used by the `exit` builtin: blocking-waits on every still-active job
 * (NOT WNOHANG this time -- exit must not return until they're all
 * really done) and frees the job table's memory.
 * NOT YET IMPLEMENTED. */
void wait_all_background_jobs(void);

#endif
