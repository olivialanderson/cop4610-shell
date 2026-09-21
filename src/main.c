#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "parser.h"
#include "executor.h"
#include "background.h"
#include "builtins.h"

/* ---- Part 1: Prompt (Gabriel + Gannon) ---------------------------------
 * Placeholder implementation so the rest of the team has a working loop
 * to build against. Gabriel/Gannon: this is yours to finalize -- in
 * particular, verify on linprog whether $MACHINE is actually a set env
 * var (echo $MACHINE there) or whether gethostname() (used below) is
 * the right source, per the handout's "USER@MACHINE:PWD>" format. */
static void print_prompt(void) {
    char *user = getenv("USER");
    if (user == NULL) user = "unknown";

    char machine[HOST_NAME_MAX + 1];
    if (gethostname(machine, sizeof(machine)) != 0) {
        snprintf(machine, sizeof(machine), "unknown");
    }

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        snprintf(cwd, sizeof(cwd), "?");
    }

    printf("%s@%s:%s> ", user, machine, cwd);
    fflush(stdout);
}

/* This main loop is deliberately a skeleton: prompt -> read -> tokenize
 * -> free, proven to work end to end (see the comprehension check in
 * README/ai-transcripts or ask your tutor). It does NOT yet call
 * expand_env/expand_tilde, find_executable, run_external,
 * run_pipeline, apply_redirection, or the real builtins -- those are
 * stubbed in their own files (parser.c, executor.c, pipes.c,
 * redirection.c, builtins.c, background.c) with TODOs matching the
 * division of labor in divison_of_labor.md. Wire each one in as you
 * implement it, one at a time, and re-test the loop after each change. */
int main(void) {
    while (1) {
        check_background_jobs(); /* Part 8 -- currently a no-op stub */
        print_prompt();

        char *line = read_line();
        if (line == NULL) {
            printf("\n");
            break; /* EOF (Ctrl+D) */
        }

        char **tokens = tokenize(line);

        if (tokens == NULL) {
            free(line);
            continue; /* blank line */
        }
	for (int i = 0; tokens[i]; i++) {
            char *expanded = expand_env(tokens[i]);
            free(tokens[i]);
            tokens[i] = expanded;
        }

        /* Placeholder dispatch so the loop is exercisable end to end.
         * Once builtin_exit/cd/jobs are implemented, replace this
         * strcmp with real builtin-vs-external dispatch (see
         * builtins.h). */
        if (strcmp(tokens[0], "exit") == 0) {
            for (int i = 0; tokens[i]; i++) free(tokens[i]);
            free(tokens);
            free(line);
            break;
        }

        printf("[parsed tokens]");
        for (int i = 0; tokens[i]; i++) {
            printf(" [%d]=\"%s\"", i, tokens[i]);
        }
        printf("\n");

        for (int i = 0; tokens[i]; i++) free(tokens[i]);
        free(tokens);
        free(line);
    }
    return 0;
}
