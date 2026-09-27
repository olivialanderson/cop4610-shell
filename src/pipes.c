#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "pipes.h"

/* ---- Part 7: Gabriel + Gannon ------------------------------------------
 *  * Stubbed so the rest of the project keeps compiling. See pipes.h for
 *   * the fd-wiring contract (n-1 pipes, n forks, which fds to dup2/close in
 *    * which child) and design-notes.md for the full walkthrough -- the TA
 *     * slides specifically warn that forgetting to close the unused pipe
 *      * ends is the most common bug here. */

int run_pipeline(cmd_t *cmds, int n, int background) {
    if (cmds == NULL || n <= 0) {
        fprintf(stderr, "run_pipeline: invalid pipeline\n");
        return -1;
    }

    /*
 *      * Part 7 only requires a maximum of two pipes,
 *           * which means a maximum of three commands.
 *                */
    if (n > 3) {
        fprintf(stderr, "run_pipeline: maximum of two pipes supported\n");
        return -1;
    }

    /*
 *      * n commands require n-1 pipes.
 *           *
 *                * For example:
 *                     *
 *                          *   cmd1 | cmd2
 *                               *
 *                                    * requires one pipe.
 *                                         *
 *                                              *   cmd1 | cmd2 | cmd3
 *                                                   *
 *                                                        * requires two pipes.
 *                                                             */
    int pipes[2][2];

    for (int i = 0; i < n - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");

            /* Close any pipes that were already created. */
            for (int j = 0; j < i; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            return -1;
        }
    }

    /*
 *      * Store the PID of every child so the parent can wait for
 *           * all processes in the foreground pipeline.
 *                */
    pid_t pids[3];

    /*
 *      * Fork once for every command in the pipeline.
 *           */
    for (int i = 0; i < n; i++) {
        pids[i] = fork();

        if (pids[i] == -1) {
            perror("fork");

            /*
 *              * Close all pipe file descriptors in the parent.
 *                           */
            for (int j = 0; j < n - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            /*
 *              * Wait for children that were already created.
 *                           */
            for (int j = 0; j < i; j++) {
                waitpid(pids[j], NULL, 0);
            }

            return -1;
        }

        /*
 *          * CHILD
 *                   */
        if (pids[i] == 0) {

            /*
 *              * If this is not the first command, its stdin comes
 *                           * from the read end of the previous pipe.
 *                                        *
 *                                                     * cmd2 in:
 *                                                                  *
 *                                                                               *     cmd1 | cmd2
 *                                                                                            *
 *                                                                                                         * receives stdin from pipe[0].
 *                                                                                                                      */
            if (i > 0) {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2 stdin");
                    _exit(1);
                }
            }

            /*
 *              * If this is not the last command, its stdout goes
 *                           * to the write end of the current pipe.
 *                                        *
 *                                                     * cmd1 in:
 *                                                                  *
 *                                                                               *     cmd1 | cmd2
 *                                                                                            *
 *                                                                                                         * sends stdout into pipe[0].
 *                                                                                                                      */
            if (i < n - 1) {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2 stdout");
                    _exit(1);
                }
            }

            /*
 *              * VERY IMPORTANT:
 *                           *
 *                                        * Close every pipe file descriptor in the child.
 *                                                     *
 *                                                                  * After dup2(), stdin/stdout now refer to the appropriate
 *                                                                               * pipe ends, so the original descriptors are no longer
 *                                                                                            * needed.
 *                                                                                                         *
 *                                                                                                                      * Leaving unused pipe descriptors open can cause commands
 *                                                                                                                                   * such as `cat | wc` to wait forever for EOF.
 *                                                                                                                                                */
            for (int j = 0; j < n - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            /*
 *              * The assignment guarantees that piping and I/O
 *                           * redirection do not occur together, so there is no
 *                                        * redirection to apply here.
 *                                                     *
 *                                                                  * cmds[i].argv has already been resolved with
 *                                                                               * find_executable(), according to pipes.h.
 *                                                                                            */

            if (cmds[i].argv == NULL || cmds[i].argv[0] == NULL) {
                fprintf(stderr, "run_pipeline: invalid command\n");
                _exit(1);
            }

            /*
 *              * The project restriction allows fork() and execv().
 *                           * Use execv() directly because this child is already
 *                                        * the process that should execute the command.
 *                                                     */
            execv(cmds[i].argv[0], cmds[i].argv);

            /*
 *              * execv() only returns if there was an error.
 *                           */
            perror(cmds[i].argv[0]);
            _exit(127);
        }
    }

    /*
 *      * PARENT
 *           *
 *                * Once all children have been forked, the parent does not need
 *                     * any pipe file descriptors.
 *                          *
 *                               * Closing them is essential so that the children can eventually
 *                                    * receive EOF through the pipeline.
 *                                         */
    for (int i = 0; i < n - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    /*
 *      * Foreground pipeline:
 *           *
 *                * Wait for every command in the pipeline.
 *                     */
    if (!background) {
        int status;
        int last_status = 0;

        for (int i = 0; i < n; i++) {
            if (waitpid(pids[i], &status, 0) == -1) {
                if (errno == EINTR) {
                    i--;
                    continue;
                }

                perror("waitpid");
                return -1;
            }

            /*
 *              * Save the status of the final command.
 *                           *
 *                                        * A pipeline's status is normally based on its last
 *                                                     * command.
 *                                                                  */
            if (i == n - 1) {
                last_status = status;
            }
        }

        if (WIFEXITED(last_status)) {
            return WEXITSTATUS(last_status);
        }

        if (WIFSIGNALED(last_status)) {
            return 128 + WTERMSIG(last_status);
        }

        return 0;
    }

    /*
 *      * Background pipeline:
 *           *
 *                * Do not wait here. The background-processing portion of the
 *                     * project can track the child processes separately.
 *                          */
    return 0;
}
