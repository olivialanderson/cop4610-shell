#include <stdio.h>
#include <errno.h>
#include "redirection.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "executor.h"
#include <unistd.h>
#include <sys/wait.h>

/* Part 4: resolve executable paths without changing the environment. */
static int is_executable_file(const char *path) {
    struct stat info;
    return stat(path, &info) == 0 && S_ISREG(info.st_mode)
        && access(path, X_OK) == 0;
}

char *find_executable(const char *cmd) {
    if (cmd == NULL || cmd[0] == '\0') {
        return NULL;
    }

    /* Commands containing a slash bypass PATH entirely. */
    if (strchr(cmd, '/') != NULL) {
        return is_executable_file(cmd) ? strdup(cmd) : NULL;
    }

    const char *path = getenv("PATH");
    if (path == NULL) {
        return NULL;
    }
    char *path_copy = strdup(path);
    if (path_copy == NULL) {
        return NULL;
    }

    char *directory = path_copy;
    while (1) {
        char *separator = strchr(directory, ':');
        if (separator != NULL) {
            *separator = '\0';
        }

        /* Empty PATH entries mean the current directory. */
        const char *prefix = directory[0] == '\0' ? "." : directory;
        size_t size = strlen(prefix) + strlen(cmd) + 2;
        char *candidate = malloc(size);
        if (candidate == NULL) {
            free(path_copy);
            return NULL;
        }
        snprintf(candidate, size, "%s/%s", prefix, cmd);
        if (is_executable_file(candidate)) {
            free(path_copy);
            return candidate;
        }
        free(candidate);

        if (separator == NULL) {
            break;
        }
        directory = separator + 1;
    }
    free(path_copy);
    return NULL;
}

int run_external(char **argv) {
    return run_external_redirected(argv, NULL, NULL);
}

int run_external_redirected(char **argv, const char *infile, const char *outfile) {
    pid_t pid = fork();

    if (pid == 0) {
        if (apply_redirection(infile, outfile) == -1) {
            _exit(1);
        }
        execv(argv[0], argv);
        perror("execv");
        _exit(1);
    } else if (pid > 0) {
        int status;
        while (waitpid(pid, &status, 0) == -1) {
            if (errno == EINTR) continue;
            perror("waitpid");
            return -1;
        }
        return status;
    } else {
        perror("fork");
        return -1;
    }
}
