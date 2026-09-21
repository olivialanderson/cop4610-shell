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

/* Expand tokens, handle builtins, then resolve and run external commands.
 * Pipelines, redirection, and background execution still need integration. */
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
            /* Expand the original token once; do not reinterpret values. */
            char *expanded = tokens[i][0] == '$'
                ? expand_env(tokens[i]) : expand_tilde(tokens[i]);
            if (expanded == NULL) {
                perror("token expansion");
                for (int j = 0; tokens[j]; j++) free(tokens[j]);
                free(tokens);
                free(line);
                return 1;
            }
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

        /* Builtins must bypass PATH search, even while they are stubs. */
        if (strcmp(tokens[0], "cd") == 0) {
            builtin_cd(tokens);
        } else if (strcmp(tokens[0], "jobs") == 0) {
            builtin_jobs(tokens);
        } else {
            char *executable = find_executable(tokens[0]);
            if (executable == NULL) {
                fprintf(stderr, "%s: command not found or not executable\n", tokens[0]);
            } else {
                free(tokens[0]);
                tokens[0] = executable;
                run_external(tokens);
            }
        }

        for (int i = 0; tokens[i]; i++) free(tokens[i]);
        free(tokens);
        free(line);
    }
    return 0;
}
