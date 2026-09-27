#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "builtins.h"
#include "background.h"
static char *history[3];
static int history_count;

int is_builtin(const char *name) {
    return strcmp(name, "cd") == 0 || strcmp(name, "jobs") == 0
        || strcmp(name, "exit") == 0;
}
int builtin_cd(char **argv) {
    if (argv[1] != NULL && argv[2] != NULL) {
        fprintf(stderr, "cd: too many arguments\n");
        return 1;
    }
    const char *target = argv[1] == NULL ? getenv("HOME") : argv[1];
    if (target == NULL) {
        fprintf(stderr, "cd: HOME is not set\n");
        return 1;
    }
    if (chdir(target) == -1) {
        perror("cd");
        return 1;
    }
    /* Keep environment expansion consistent with the directory in the prompt. */
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("cd: getcwd");
        return 1;
    }
    if (setenv("PWD", cwd, 1) == -1) {
        perror("cd: PWD");
        return 1;
    }
    return 0;
}
int builtin_jobs(char **argv) {
    if (argv[1] != NULL) {
        fprintf(stderr, "jobs: no arguments expected\n");
        return 1;
    }
    print_jobs();
    return 0;
}
void record_valid_command(const char *cmdline) {
    char *copy = strdup(cmdline);
    if (copy == NULL) {
        perror("command history");
        return;
    }
    if (history_count == 3) {
        free(history[0]);
        history[0] = history[1];
        history[1] = history[2];
        history_count--;
    }
    history[history_count++] = copy;
}
int builtin_exit(char **argv) {
    if (argv != NULL && argv[1] != NULL) {
        fprintf(stderr, "exit: no arguments expected\n");
        return 1;
    }
    wait_all_background_jobs();
    puts("Command history:");
    if (history_count == 0) puts("no valid commands");
    else if (history_count < 3) puts(history[history_count - 1]);
    else for (int i = 0; i < history_count; i++) puts(history[i]);
    for (int i = 0; i < history_count; i++) free(history[i]);
    history_count = 0;
    return 0;
}
