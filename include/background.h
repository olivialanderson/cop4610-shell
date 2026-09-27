#ifndef BACKGROUND_H
#define BACKGROUND_H
#include <sys/types.h>
#define MAX_BG_JOBS 10
/* Reserve storage before forking; commit only after all children start. */
int reserve_background_job(int count, const char *cmdline);
void cancel_background_job(int slot);
void start_background_job(int slot, const pid_t *pids);
void check_background_jobs(void);
void print_jobs(void);
void wait_all_background_jobs(void);
#endif
