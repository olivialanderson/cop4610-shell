#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

/* ---- implemented now: the lexer/tokenizer, so the loop is testable --- */

char *read_line(void) {
    char *buf = NULL;
    size_t cap = 0;
    ssize_t len = getline(&buf, &cap, stdin);
    if (len == -1) {
        free(buf);
        return NULL; /* EOF or read error */
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }
    return buf;
}

char **tokenize(const char *line) {
    char **tokens = malloc(sizeof(char *) * (MAX_TOKENS + 1));
    if (tokens == NULL) {
        perror("malloc");
        exit(1);
    }

    int count = 0;
    char *copy = strdup(line);
    if (copy == NULL) {
        perror("strdup");
        free(tokens);
        exit(1);
    }

    char *saveptr = NULL;
    char *tok = strtok_r(copy, " \t", &saveptr);
    while (tok != NULL && count < MAX_TOKENS) {
        tokens[count] = strdup(tok);
        if (tokens[count] == NULL) {
            perror("strdup");
            exit(1);
        }
        count++;
        tok = strtok_r(NULL, " \t", &saveptr);
    }
    tokens[count] = NULL;
    free(copy);

    if (count == 0) {
        free(tokens);
        return NULL;
    }
    return tokens;
}

/* ---- Part 2/3: Olivia + Gannon's work starts here ---------------------
 * These are stubbed so the project keeps compiling for everyone else
 * while you two implement the real behavior. Right now they just hand
 * back an unmodified copy of the token, which is why running the shell
 * won't actually expand $USER or ~ yet. See parser.h for the exact
 * contract (what each function must return in each case) and
 * design-notes.md in the course project for a worked trace. */

char *expand_env(const char *token) {
    /* TODO (Part 2, Olivia + Gannon): if token starts with '$', look it
     * up with getenv() and return a heap copy of the value (or "" if
     * unset). Remember: never return getenv()'s pointer directly, and
     * never write through it. */
    return strdup(token);
}

char *expand_tilde(const char *token) {
    /* TODO (Part 3, Olivia + Gannon): handle exactly "~" and the "~/..."
     * prefix case by substituting getenv("HOME"). Leave every other
     * token untouched. */
    return strdup(token);
}
