#ifndef PIPES_H
#define PIPES_H
/* argv owns expanded strings; redirection names are separately owned. */
typedef struct {
    char **argv;
    char *infile;
    char *outfile;
} cmd_t;
/* Returns 0 on foreground success/background launch; nonzero on failure. */
int run_pipeline(cmd_t *cmds, int n, int background, const char *cmdline);
#endif
