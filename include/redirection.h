#ifndef REDIRECTION_H
#define REDIRECTION_H

/* ---- Part 6: I/O Redirection (Gabriel + Gannon) -----------------------
 * `infile` / `outfile` are NULL if not present for this command.
 * Should be called from the CHILD process (after fork, before execv):
 *  - infile: open(infile, O_RDONLY) -- error if it doesn't exist or
 *    isn't a regular file -- then dup2 onto STDIN_FILENO (fd 0).
 *  - outfile: open(outfile, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)
 *    -- that mode is exactly "-rw-------" per the slide -- then dup2
 *    onto STDOUT_FILENO (fd 1). O_TRUNC means overwrite, not append,
 *    which is what the handout requires.
 * Returns 0 on success, -1 on failure (caller should report the error
 * and avoid calling execv()).
 * NOT YET IMPLEMENTED. */
int apply_redirection(const char *infile, const char *outfile);

#endif
