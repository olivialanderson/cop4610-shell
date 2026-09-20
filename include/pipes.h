#ifndef PIPES_H
#define PIPES_H

#include "parser.h"

/* One command in a (possibly length-1) pipeline. */
typedef struct {
    char **argv;      /* NULL-terminated argv, already resolved via find_executable() */
    char *infile;      /* NULL if none */
    char *outfile;     /* NULL if none */
} cmd_t;

/* ---- Part 7: Piping (Gabriel + Gannon) --------------------------------
 * Runs `n` commands (n==1 means "no pipe, just run it") wired together:
 * cmds[i]'s stdout -> cmds[i+1]'s stdin. Handles up to 2 pipes (3
 * commands) per the handout's required scope; extra credit is unlimited.
 *
 * You'll need n-1 pipe() calls and n fork() calls. In child i:
 *   if i > 0:     dup2(pipe[i-1].read_end,  STDIN_FILENO)
 *   if i < n-1:    dup2(pipe[i].write_end,   STDOUT_FILENO)
 * then close every pipe fd you're not using in that child (a very common
 * bug per the TA slides is forgetting this and getting no output), then
 * apply_redirection() if cmds[i] has one, then run_external().
 * Parent closes all pipe fds after forking everyone, then waitpid()s on
 * each child (unless the whole pipeline is backgrounded -- see
 * background.c) and should track/report the LAST command's pid for
 * backgrounded piped jobs per the slide.
 * NOT YET IMPLEMENTED. */
int run_pipeline(cmd_t *cmds, int n, int background);

#endif
