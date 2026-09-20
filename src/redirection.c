#include "redirection.h"

/* ---- Part 6: Gabriel + Gannon ------------------------------------------
 * Stubbed so the rest of the project keeps compiling. See redirection.h
 * for the exact open()/dup2() contract (including the -rw------- mode
 * requirement) and design-notes.md for the guideline on cmd < in > out
 * vs cmd > out < in ordering. */

int apply_redirection(const char *infile, const char *outfile) {
    (void)infile;
    (void)outfile;
    /* TODO (Part 6): open()/dup2() infile onto STDIN and/or outfile onto
     * STDOUT as described in redirection.h. */
    return 0;
}
