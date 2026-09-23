#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "parser.h"
#include "executor.h"
#include "background.h"
#include "builtins.h"
#include "redirection.h"

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

/* Return owned expanded text, leaving syntax recognition to raw tokens. */
static char *expand_word(const char *word) {
    char *result = word[0] == '$' ? expand_env(word) : expand_tilde(word);
    if (result == NULL) {
        perror("token expansion");
        exit(1);
    }
    return result;
}

static int is_operator(const char *word) {
    return word[0] != '\0' && word[1] == '\0'
        && strchr("<>|&", word[0]) != NULL;
}

/* Preserve the shell's own descriptors when redirecting a builtin. */
static int run_builtin(char **argv, const char *infile, const char *outfile) {
    int saved_in = dup(STDIN_FILENO);
    int saved_out = dup(STDOUT_FILENO);
    int should_exit = 0;
    if (saved_in == -1 || saved_out == -1) {
        perror("save shell descriptors");
        if (saved_in != -1) close(saved_in);
        if (saved_out != -1) close(saved_out);
        return 0;
    }
    fflush(stdout);
    if (apply_redirection(infile, outfile) == 0) {
        if (strcmp(argv[0], "cd") == 0) builtin_cd(argv);
        else if (strcmp(argv[0], "jobs") == 0) builtin_jobs(argv);
        else should_exit = 1; /* Full Part 9 exit behavior remains unfinished. */
    }
    fflush(stdout);
    if (dup2(saved_in, STDIN_FILENO) == -1 || dup2(saved_out, STDOUT_FILENO) == -1) {
        perror("restore shell descriptors");
        exit(1);
    }
    close(saved_in);
    close(saved_out);
    return should_exit;
}

int main(void) {
    while (1) {
        check_background_jobs();
        print_prompt();
        char *line = read_line();
        if (line == NULL) {
            printf("\n");
            break;
        }
        char **tokens = tokenize(line);
        if (tokens == NULL) {
            free(line);
            continue;
        }

        char *argv[MAX_TOKENS + 1];
        int argc = 0;
        int valid = 1;
        int should_exit = 0;
        char *infile = NULL;
        char *outfile = NULL;

        /* Remove operators and filenames from argv before executing. */
        for (int i = 0; tokens[i] != NULL; i++) {
            if (strcmp(tokens[i], "<") == 0 || strcmp(tokens[i], ">") == 0) {
                char **target = tokens[i][0] == '<' ? &infile : &outfile;
                if (*target != NULL || tokens[i + 1] == NULL
                    || is_operator(tokens[i + 1])) {
                    fprintf(stderr, "redirection: missing filename or repeated operator\n");
                    valid = 0;
                    break;
                }
                *target = expand_word(tokens[++i]);
            } else if (is_operator(tokens[i])) {
                fprintf(stderr, "pipes and background processing are not implemented yet\n");
                valid = 0;
                break;
            } else {
                argv[argc++] = expand_word(tokens[i]);
            }
        }
        argv[argc] = NULL;
        if (valid && argc == 0) {
            fprintf(stderr, "redirection: missing command\n");
            valid = 0;
        }
        if (valid) {
            if (strcmp(argv[0], "exit") == 0 || strcmp(argv[0], "cd") == 0
                || strcmp(argv[0], "jobs") == 0) {
                should_exit = run_builtin(argv, infile, outfile);
            } else {
                char *executable = find_executable(argv[0]);
                if (executable == NULL) {
                    fprintf(stderr, "%s: command not found or not executable\n", argv[0]);
                } else {
                    free(argv[0]);
                    argv[0] = executable;
                    run_external_redirected(argv, infile, outfile);
                }
            }
        }
        for (int i = 0; i < argc; i++) free(argv[i]);
        for (int i = 0; tokens[i]; i++) free(tokens[i]);
        free(tokens);
        free(infile);
        free(outfile);
        free(line);
        if (should_exit) break;
    }
    return 0;
}
