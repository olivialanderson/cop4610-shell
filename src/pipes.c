#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include "pipes.h"
#include "background.h"
#include "builtins.h"
#include "redirection.h"

int run_pipeline(cmd_t *cmds, int n, int background, const char *cmdline) {
    pid_t *pids = calloc((size_t)n, sizeof(pid_t));
    if (pids == NULL) { perror("pipeline allocation"); return 1; }
    int slot = background ? reserve_background_job(n, cmdline) : -1;
    if (background && slot == -1) { free(pids); return 1; }
    int previous = -1;
    int started = 0;
    fflush(NULL);
    for (int i = 0; i < n; i++) {
        int channel[2] = {-1, -1};
        if (i < n - 1 && pipe(channel) == -1) {
            perror("pipe");
            goto failure;
        }
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            if (channel[0] != -1) close(channel[0]);
            if (channel[1] != -1) close(channel[1]);
            goto failure;
        }
        if (pid == 0) {
            if ((previous != -1 && dup2(previous, STDIN_FILENO) == -1)
                || (channel[1] != -1 && dup2(channel[1], STDOUT_FILENO) == -1)) {
                perror("pipe redirection");
                _exit(1);
            }
            if (previous != -1) close(previous);
            if (channel[0] != -1) close(channel[0]);
            if (channel[1] != -1) close(channel[1]);
            if (apply_redirection(cmds[i].infile, cmds[i].outfile) == -1) _exit(1);
            /* Pipeline builtins run in a child and cannot change the parent. */
            if (is_builtin(cmds[i].argv[0])) {
                int status = 0;
                if (strcmp(cmds[i].argv[0], "cd") == 0)
                    status = builtin_cd(cmds[i].argv);
                else if (strcmp(cmds[i].argv[0], "jobs") == 0)
                    status = builtin_jobs(cmds[i].argv);
                fflush(NULL);
                _exit(status);
            }
            execv(cmds[i].argv[0], cmds[i].argv);
            perror(cmds[i].argv[0]);
            _exit(127);
        }
        pids[started++] = pid;
        if (previous != -1) close(previous);
        if (channel[1] != -1) close(channel[1]);
        previous = channel[0];
    }
    if (background) {
        start_background_job(slot, pids);
        free(pids);
        return 0;
    }
    int failed = 0;
    for (int i = 0; i < n; i++) {
        int status;
        pid_t result;
        do { result = waitpid(pids[i], &status, 0); } while (result < 0 && errno == EINTR);
        if (result < 0) { perror("waitpid"); failed = 1; }
        else if (i == n - 1 && (!WIFEXITED(status) || WEXITSTATUS(status) != 0)) failed = 1;
    }
    free(pids);
    return failed;

failure:
    if (previous != -1) close(previous);
    /* A partial pipeline must not leave children blocked on missing peers. */
    for (int i = 0; i < started; i++) kill(pids[i], SIGKILL);
    for (int i = 0; i < started; i++) {
        while (waitpid(pids[i], NULL, 0) == -1 && errno == EINTR) {}
    }
    if (slot != -1) cancel_background_job(slot);
    free(pids);
    return 1;
}
