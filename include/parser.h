#ifndef PARSER_H
#define PARSER_H

#define MAX_TOKENS 128

/* ---- Part 0/1 groundwork (buildable now) ------------------------------ */

/* Reads one line from stdin. Caller owns the returned buffer and must
 * free() it. Returns NULL on EOF (Ctrl+D). */
char *read_line(void);

/* Splits whitespace and separates <, >, |, & into a NULL-terminated array of heap-owned
 * C-strings. Caller owns the array AND every string inside it:
 *   for (int i = 0; tokens[i]; i++) free(tokens[i]);
 *   free(tokens);
 * Returns NULL if the line was empty/whitespace-only.
 *
 * Expansion and command parsing are performed by the caller. */
char **tokenize(const char *line);

/* ---- Part 2: Environment Variables (Olivia + Gannon) ------------------ */

/* If `token` is exactly "$NAME", return a newly heap-allocated copy of
 * getenv("NAME") (or an empty string "" if that variable is unset -- the
 * handout says an undefined variable "evaluates to nothing", not an
 * error). If `token` does not start with '$', return a heap copy of
 * `token` unchanged. Never return the pointer getenv() gave you directly:
 * always build a fresh copy, since callers will free() whatever you
 * return and you must never mutate/free what getenv() returned.
 * NOT YET IMPLEMENTED -- see design-notes.md in the course project for
 * the worked example and edge cases (undefined var, non-$ token). */
char *expand_env(const char *token);

/* ---- Part 3: Tilde Expansion (Olivia + Gannon) ------------------------ */

/* If `token` is exactly "~", return a heap copy of getenv("HOME").
 * If `token` starts with "~/", return a heap copy of getenv("HOME")
 * followed by the rest of token after the "~". Otherwise return a heap
 * copy of `token` unchanged (the handout says we do NOT need to handle
 * "~dir1", "dir1~", "~+/dir1", etc.).
 * If HOME is unset, return a heap copy of the token unchanged.
 * Returns NULL if allocation fails. Caller must free the result. */
char *expand_tilde(const char *token);

#endif
