#ifndef REDIRECTION_H
#define REDIRECTION_H

/* NULL filenames mean no redirection. Validate regular-file input first,
 * then create/truncate output with mode 0600 (also for existing files).
 * Apply to the child before execv, or save/restore shell fds for builtins.
 * Reports errors itself; returns 0 on success and -1 on failure. */
int apply_redirection(const char *infile, const char *outfile);

#endif
