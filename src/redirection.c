#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "redirection.h"

int apply_redirection(const char *infile, const char *outfile) {
    int input = -1;
    int output = -1;
    struct stat info;

    /* Validate input before creating or truncating output, in either order. */
    if (infile != NULL) {
        input = open(infile, O_RDONLY | O_NONBLOCK);
        if (input == -1) {
            perror(infile);
            goto error;
        }
        if (fstat(input, &info) == -1) {
            perror(infile);
            goto error;
        }
        if (!S_ISREG(info.st_mode)) {
            fprintf(stderr, "%s: input must be a regular file\n", infile);
            goto error;
        }
    }
    if (outfile != NULL) {
        output = open(outfile, O_WRONLY | O_CREAT | O_NONBLOCK, 0600);
        if (output == -1) {
            perror(outfile);
            goto error;
        }
        struct stat output_info;
        if (fstat(output, &output_info) == -1) {
            perror(outfile);
            goto error;
        }
        if (!S_ISREG(output_info.st_mode)) {
            fprintf(stderr, "%s: output must be a regular file\n", outfile);
            goto error;
        }
        if (input != -1 && info.st_dev == output_info.st_dev
            && info.st_ino == output_info.st_ino) {
            fprintf(stderr, "redirection: input and output refer to the same file\n");
            goto error;
        }
        /* open's mode does not change existing files and is affected by umask. */
        if (fchmod(output, 0600) == -1) {
            perror(outfile);
            goto error;
        }
        if (ftruncate(output, 0) == -1) {
            perror(outfile);
            goto error;
        }
    }
    if (input != -1 && dup2(input, STDIN_FILENO) == -1) {
        perror("input redirection");
        goto error;
    }
    if (output != -1 && dup2(output, STDOUT_FILENO) == -1) {
        perror("output redirection");
        goto error;
    }
    if (input != -1 && input != STDIN_FILENO) close(input);
    if (output != -1 && output != STDOUT_FILENO) close(output);
    return 0;

error:
    if (input != -1) close(input);
    if (output != -1) close(output);
    return -1;
}
