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

/* ---- Part 1: Prompt (Gabriel + Gannon) ---------------------------------
 *  * Placeholder implementation so the rest of the team has a working loop
 *   * to build against. Gabriel/Gannon: this is yours to finalize -- in
 *    * particular, verify on linprog whether $MACHINE is actually a set env
 *     * var (echo $MACHINE there) or whether gethostname() (used below) is
 *      * the right source, per the handout's "USER@MACHINE:PWD>" format. */
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
        if (strcmp(argv[0], "cd") == 0)
            builtin_cd(argv);
        else if (strcmp(argv[0], "jobs") == 0)
            builtin_jobs(argv);
        else
            should_exit = 1; /* Full Part 9 exit behavior remains unfinished. */
    }

    fflush(stdout);

    if (dup2(saved_in, STDIN_FILENO) == -1 ||
        dup2(saved_out, STDOUT_FILENO) == -1) {
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

        /*
 *          * ---------------------------------------------------------------
 *                   * Part 7: Piping
 *                            * ---------------------------------------------------------------
 *                                     *
 *                                              * The project supports a maximum of two pipes, meaning a
 *                                                       * maximum of three commands:
 *                                                                *
 *                                                                         *     cmd1 | cmd2
 *                                                                                  *
 *                                                                                           *     cmd1 | cmd2 | cmd3
 *                                                                                                    *
 *                                                                                                             * We first check whether the command contains a pipe.
 *                                                                                                                      */
        int has_pipe = 0;

        for (int i = 0; tokens[i] != NULL; i++) {
            if (strcmp(tokens[i], "|") == 0) {
                has_pipe = 1;
                break;
            }
        }

        /*
 *          * If this command contains a pipe, handle it separately from
 *                   * normal commands and Part 6 redirection.
 *                            */
        if (has_pipe) {
            cmd_t cmds[3];
            char *pipeline_argv[3][MAX_TOKENS + 1];

            memset(cmds, 0, sizeof(cmds));
            memset(pipeline_argv, 0, sizeof(pipeline_argv));

            int command_count = 1;
            int argc = 0;
            int valid = 1;

            /*
 *              * Parse each command separated by '|'.
 *                           */
            for (int i = 0; tokens[i] != NULL; i++) {

                /*
 *                  * A pipe separates two commands.
 *                                   */
                if (strcmp(tokens[i], "|") == 0) {

                    /*
 *                      * Reject:
 *                                           *
 *                                                                *     | ls
 *                                                                                     *     ls |
 *                                                                                                          *     ls || wc
 *                                                                                                                               */
                    if (argc == 0) {
                        fprintf(stderr, "pipe: missing command\n");
                        valid = 0;
                        break;
                    }

                    /*
 *                      * We only support a maximum of two pipes.
 *                                           */
                    if (command_count >= 3) {
                        fprintf(stderr,
                                "pipe: maximum of two pipes supported\n");
                        valid = 0;
                        break;
                    }

                    pipeline_argv[command_count - 1][argc] = NULL;

                    command_count++;
                    argc = 0;

                    continue;
                }

                /*
 *                  * Part 7 and Part 6 are separate according to the
 *                                   * assignment assumptions. Piping and I/O redirection
 *                                                    * will not occur together.
 *                                                                     */
                if (strcmp(tokens[i], "<") == 0 ||
                    strcmp(tokens[i], ">") == 0) {
                    fprintf(stderr,
                            "pipe: piping and redirection cannot be used together\n");
                    valid = 0;
                    break;
                }

                /*
 *                  * Background processing is handled separately in the
 *                                   * later part of the project.
 *                                                    */
                if (strcmp(tokens[i], "&") == 0) {
                    fprintf(stderr,
                            "pipe: background processing is not implemented yet\n");
                    valid = 0;
                    break;
                }

                /*
 *                  * Normal command argument.
 *                                   */
                if (argc >= MAX_TOKENS) {
                    fprintf(stderr, "pipe: too many arguments\n");
                    valid = 0;
                    break;
                }

                pipeline_argv[command_count - 1][argc++] =
                    expand_word(tokens[i]);
            }

            /*
 *              * Make sure the final command contains something.
 *                           *
 *                                        * This catches:
 *                                                     *
 *                                                                  *     ls |
 *                                                                               */
            if (valid && argc == 0) {
                fprintf(stderr, "pipe: missing command\n");
                valid = 0;
            }

            if (valid) {
                pipeline_argv[command_count - 1][argc] = NULL;

                /*
 *                  * Resolve every command using find_executable().
 *                                   *
 *                                                    * run_pipeline() expects argv[0] to already contain
 *                                                                     * the executable path.
 *                                                                                      */
                for (int c = 0; c < command_count; c++) {
                    if (pipeline_argv[c][0] == NULL) {
                        fprintf(stderr, "pipe: missing command\n");
                        valid = 0;
                        break;
                    }

                    char *executable =
                        find_executable(pipeline_argv[c][0]);

                    if (executable == NULL) {
                        fprintf(stderr,
                                "%s: command not found or not executable\n",
                                pipeline_argv[c][0]);
                        valid = 0;
                        break;
                    }

                    free(pipeline_argv[c][0]);
                    pipeline_argv[c][0] = executable;

                    cmds[c].argv = pipeline_argv[c];
                    cmds[c].infile = NULL;
                    cmds[c].outfile = NULL;
                }
            }

            /*
 *              * Run the complete pipeline in the foreground.
 *                           *
 *                                        * Background processing is not being implemented as part
 *                                                     * of Part 7 yet, so background = 0.
 *                                                                  */
            if (valid) {
                run_pipeline(cmds, command_count, 0);
            }

            /*
 *              * Free all expanded pipeline arguments.
 *                           */
            for (int c = 0; c < command_count; c++) {
                for (int a = 0; pipeline_argv[c][a] != NULL; a++) {
                    free(pipeline_argv[c][a]);
                }
            }

            /*
 *              * Free the parser's original tokens.
 *                           */
            for (int i = 0; tokens[i] != NULL; i++) {
                free(tokens[i]);
            }

            free(tokens);
            free(line);

            /*
 *              * Start the next shell command.
 *                           */
            continue;
        }

        /*
 *          * ---------------------------------------------------------------
 *                   * Normal command processing
 *                            * ---------------------------------------------------------------
 *                                     *
 *                                              * This is the existing Part 6 / Part 9 logic.
 *                                                       */
        char *argv[MAX_TOKENS + 1];
        int argc = 0;
        int valid = 1;
        int should_exit = 0;

        char *infile = NULL;
        char *outfile = NULL;

        /* Remove operators and filenames from argv before executing. */
        for (int i = 0; tokens[i] != NULL; i++) {

            if (strcmp(tokens[i], "<") == 0 ||
                strcmp(tokens[i], ">") == 0) {

                char **target =
                    tokens[i][0] == '<' ? &infile : &outfile;

                if (*target != NULL ||
                    tokens[i + 1] == NULL ||
                    is_operator(tokens[i + 1])) {

                    fprintf(stderr,
                            "redirection: missing filename or repeated operator");
                    valid = 0;
                    break;
                }

                *target = expand_word(tokens[++i]);

            } else if (is_operator(tokens[i])) {

                /*
 *                  * The only operator handled above in this path is
 *                                   * redirection. Pipes were already handled by the
 *                                                    * Part 7 section.
 *                                                                     */
                fprintf(stderr,
                        "pipes and background processing are not implemented yet\n");
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

            /*
 *              * Built-in commands run inside the shell process.
 *                           */
            if (strcmp(argv[0], "exit") == 0 ||
                strcmp(argv[0], "cd") == 0 ||
                strcmp(argv[0], "jobs") == 0) {

                should_exit =
                    run_builtin(argv, infile, outfile);

            } else {

                /*
 *                  * External command.
 *                                   */
                char *executable =
                    find_executable(argv[0]);

                if (executable == NULL) {

                    fprintf(stderr,
                            "%s: command not found or not executable\n",
                            argv[0]);

                } else {

                    free(argv[0]);
                    argv[0] = executable;

                    run_external_redirected(
                        argv,
                        infile,
                        outfile
                    );
                }
            }
        }

        /*
 *          * Free normal command arguments.
 *                   */
        for (int i = 0; i < argc; i++) {
            free(argv[i]);
        }

        /*
 *          * Free parser tokens.
 *                   */
        for (int i = 0; tokens[i]; i++) {
            free(tokens[i]);
        }

        free(tokens);
        free(infile);
        free(outfile);
        free(line);

        if (should_exit)
            break;
    }

    return 0;
}
