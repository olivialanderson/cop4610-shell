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
#include "pipes.h"

static void print_prompt(void) {
    const char *user = getenv("USER");
    const char *machine = getenv("MACHINE");
    char hostname[HOST_NAME_MAX + 1];
    char cwd[PATH_MAX];
    if (user == NULL) user = "unknown";
    if (machine == NULL) {
        if (gethostname(hostname, sizeof(hostname)) == -1) strcpy(hostname, "unknown");
        hostname[sizeof(hostname) - 1] = '\0';
        machine = hostname;
    }
    if (getcwd(cwd, sizeof(cwd)) == NULL) strcpy(cwd, "?");
    printf("%s@%s:%s> ", user, machine, cwd);
    fflush(stdout);
}
static char *expand_word(const char *word) {
    char *result = word[0] == '$' ? expand_env(word) : expand_tilde(word);
    if (result == NULL) { perror("expansion"); exit(1); }
    return result;
}
static int is_operator(const char *word) {
    return word[0] != '\0' && word[1] == '\0' && strchr("<>|&", word[0]) != NULL;
}

/* Save and restore descriptors: cd must change the shell's own directory. */
static int run_builtin(cmd_t *cmd, int *should_exit) {
    int saved_in = dup(STDIN_FILENO);
    int saved_out = dup(STDOUT_FILENO);
    int result = 1;
    if (saved_in == -1 || saved_out == -1) {
        perror("save descriptors");
        if (saved_in != -1) close(saved_in);
        if (saved_out != -1) close(saved_out);
        return 1;
    }
    fflush(NULL);
    if (apply_redirection(cmd->infile, cmd->outfile) == 0) {
        if (strcmp(cmd->argv[0], "cd") == 0) result = builtin_cd(cmd->argv);
        else if (strcmp(cmd->argv[0], "jobs") == 0) result = builtin_jobs(cmd->argv);
        else {
            result = builtin_exit(cmd->argv);
            *should_exit = result == 0;
        }
    }
    fflush(NULL);
    if (dup2(saved_in, STDIN_FILENO) == -1 || dup2(saved_out, STDOUT_FILENO) == -1) {
        perror("restore descriptors");
        exit(1);
    }
    close(saved_in);
    close(saved_out);
    return result;
}

int main(void) {
    /* Do not read ahead into commands intended for a nested shell. */
    setvbuf(stdin, NULL, _IONBF, 0);
    while (1) {
        check_background_jobs();
        print_prompt();
        char *line = read_line();
        if (line == NULL) { putchar('\n'); builtin_exit(NULL); break; }
        char **tokens = tokenize(line);
        if (tokens == NULL) { free(line); continue; }
        int total = 0;
        int capacity = 1;
        while (tokens[total] != NULL) {
            if (strcmp(tokens[total], "|") == 0) capacity++;
            total++;
        }
        cmd_t *cmds = calloc((size_t)capacity, sizeof(cmd_t));
        if (cmds == NULL) { perror("commands"); exit(1); }
        int n = 1, argc = 0, valid = 1, background = 0, should_exit = 0;
        cmds[0].argv = calloc((size_t)total + 1, sizeof(char *));
        if (cmds[0].argv == NULL) { perror("arguments"); exit(1); }
        for (int i = 0; i < total; i++) {
            cmd_t *cmd = &cmds[n - 1];
            if (strcmp(tokens[i], "|") == 0) {
                if (argc == 0) { valid = 0; break; }
                n++;
                argc = 0;
                cmds[n - 1].argv = calloc((size_t)total + 1, sizeof(char *));
                if (cmds[n - 1].argv == NULL) { perror("arguments"); exit(1); }
            } else if (strcmp(tokens[i], "&") == 0) {
                if (i != total - 1 || argc == 0) { valid = 0; break; }
                background = 1;
            } else if (strcmp(tokens[i], "<") == 0 || strcmp(tokens[i], ">") == 0) {
                char **target = tokens[i][0] == '<' ? &cmd->infile : &cmd->outfile;
                if (*target != NULL || i + 1 == total || is_operator(tokens[i + 1])) {
                    valid = 0;
                    break;
                }
                *target = expand_word(tokens[++i]);
            } else cmd->argv[argc++] = expand_word(tokens[i]);
        }
        if (argc == 0) valid = 0;
        if (!valid) {
            fprintf(stderr, "syntax error: missing command/filename or misplaced operator\n");
        }
        for (int i = 0; valid && i < n; i++) {
            if (is_builtin(cmds[i].argv[0])) continue;
            char *path = find_executable(cmds[i].argv[0]);
            if (path == NULL) {
                fprintf(stderr, "%s: command not found or not executable\n", cmds[i].argv[0]);
                valid = 0;
            } else {
                free(cmds[i].argv[0]);
                cmds[i].argv[0] = path;
            }
        }
        if (valid) {
            /* Reap in the parent after input: jobs may finish while we wait. */
            check_background_jobs();
            int result;
            int record_command = 0;
            if (n == 1 && is_builtin(cmds[0].argv[0])) {
                if (background) {
                    fprintf(stderr, "builtins cannot be backgrounded\n");
                    result = 1;
                } else result = run_builtin(&cmds[0], &should_exit);
                record_command = result == 0 && !should_exit;
            } else {
                result = run_pipeline(cmds, n, background, line);
                /* A nonzero program exit status does not invalidate its command. */
                record_command = result >= 0;
            }
            /* Do not record exit itself; history describes preceding commands. */
            if (record_command) record_valid_command(line);
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; cmds[i].argv[j]; j++) free(cmds[i].argv[j]);
            free(cmds[i].argv);
            free(cmds[i].infile);
            free(cmds[i].outfile);
        }
        free(cmds);
        for (int i = 0; tokens[i]; i++) free(tokens[i]);
        free(tokens);
        free(line);
        if (should_exit) break;
    }
    return 0;
}
