#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include "background.h"

typedef struct {
    unsigned long number;
    pid_t *pids;
    pid_t last_pid;
    int count;
    char *line;
} job_t;
static job_t jobs[MAX_BG_JOBS];
static unsigned long next_number = 1;

void cancel_background_job(int slot) {
    free(jobs[slot].pids);
    free(jobs[slot].line);
    memset(&jobs[slot], 0, sizeof(jobs[slot]));
}

int reserve_background_job(int count, const char *cmdline) {
    check_background_jobs();
    for (int i = 0; i < MAX_BG_JOBS; i++) {
        if (jobs[i].pids != NULL) continue;
        jobs[i].pids = calloc((size_t)count, sizeof(pid_t));
        jobs[i].line = strdup(cmdline);
        if (jobs[i].pids == NULL || jobs[i].line == NULL) {
            perror("background job allocation");
            cancel_background_job(i);
            return -1;
        }
        jobs[i].count = count;
        return i;
    }
    fprintf(stderr, "background: maximum of 10 active jobs\n");
    return -1;
}

void start_background_job(int slot, const pid_t *pids) {
    job_t *job = &jobs[slot];
    memcpy(job->pids, pids, (size_t)job->count * sizeof(pid_t));
    job->last_pid = pids[job->count - 1];
    job->number = next_number++;
    printf("[%lu] %ld\n", job->number, (long)job->last_pid);
}

static void reap_jobs(int blocking) {
    for (int i = 0; i < MAX_BG_JOBS; i++) {
        job_t *job = &jobs[i];
        if (job->number == 0) continue;
        int remaining = 0;
        for (int j = 0; j < job->count; j++) {
            if (job->pids[j] == 0) continue;
            pid_t result;
            do {
                result = waitpid(job->pids[j], NULL, blocking ? 0 : WNOHANG);
            } while (result == -1 && errno == EINTR);
            if (result > 0 || (result == -1 && errno == ECHILD)) {
                job->pids[j] = 0;
            } else {
                if (result == -1) perror("waitpid");
                remaining++;
            }
        }
        if (remaining == 0) {
            printf("[%lu]+ done %s\n", job->number, job->line);
            cancel_background_job(i);
        }
    }
}
void check_background_jobs(void) { reap_jobs(0); }
void wait_all_background_jobs(void) { reap_jobs(1); }
void print_jobs(void) {
    int found = 0;
    for (int i = 0; i < MAX_BG_JOBS; i++) {
        if (jobs[i].number == 0) continue;
        printf("[%lu]+ %ld %s\n", jobs[i].number,
               (long)jobs[i].last_pid, jobs[i].line);
        found = 1;
    }
    if (!found) puts("no active background jobs");
}
