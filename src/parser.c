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

/* ---- Parts 2/3: Environment variable and tilde expansion ------------- */

char *expand_env(const char *token) {
    if (token[0] !='$') {
	return strdup(token);
     }
    const char *name = token + 1;
    char *value = getenv(name);   

    if (value == NULL) {
        return strdup(""); 
    }

    return strdup(value);
}

char *expand_tilde(const char *token) {
    /* Only a standalone tilde or a leading "~/" expands. */
    if (token[0] != '~' || (token[1] != '\0' && token[1] != '/')) {
        return strdup(token);
    }

    const char *home = getenv("HOME");
    if (home == NULL) {
        return strdup(token); /* Preserve the path when HOME is unset. */
    }

    /* Skip the tilde, retaining any slash and remaining path. */
    char *expanded = malloc(strlen(home) + strlen(token + 1) + 1);
    if (expanded == NULL) {
        return NULL;
    }
    strcpy(expanded, home);
    strcat(expanded, token + 1);
    return expanded;
}
