#ifndef PIPES_H
#define PIPES_H
/* argv owns expanded strings; redirection names are separately owned. */
typedef struct {
    char **argv;
    char *infile;
    char *outfile;
} cmd_t;
/* Returns the last foreground stage's exit status, or 0 for a background launch.
 * Returns -1 for allocation, pipe, fork, job-capacity, or wait failures.
 * Nonzero program exit statuses still represent valid external commands. */
int run_pipeline(cmd_t *cmds, int n, int background, const char *cmdline);
#endif
