/* Example using an unnamed pipe for unidirectional communication between
 * two child processes that communicate through file descriptors
 * 0 (stdin, i.e. "standard input") and 1 (stdout, i.e. "standard output")
 *
 *   Author: James A. Shackleford
 *     Date: January 14th, 2016
 *
 * This example shows how a shell (like bash) uses pipes to direct the
 * output from one program to the input of another.  Here we will show
 * how the shell command:
 *
 * $ ls -l | wc -l
 *
 * is setup behind the scenes.
 *
 * Specifically, the goal will be to modify the file descriptor tables of
 * both the children fork()ed by the parent to construct the following
 * pipeline:
 *
 *          (Keyboard)
 *              |
 *              |
 *              V
 *      +-------------------+
 *      | fd 0 (stdin)      |
 *      |                   |    Child Process: ls
 *      | fd 1 (write side) |
 *      +-------------------+
 *              |
 *              | (Pipe)
 *              V
 *      +-------------------+
 *      | fd 0 (read side)  |
 *      |                   |    Child Process: wc
 *      | fd 1 (stdout)     |
 *      +-------------------+
 *              |
 *              |
 *              V
 *          (Display)
 *
 *  Note: Parent does nothing but setup the pipe and fork two
 *        children that may inherit it!
 *
 *
 * Compile using:
 *   gcc -o 2-pipes-dup 2-pipes-dup.c
 */

#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define READ_SIDE 0
#define WRITE_SIDE 1

int main(void)
{
    int fd[2];

    /* create the pipe */
    if (pipe(fd) == -1) {
        fprintf(stderr, "error -- failed to create pipe");
        return 1;
    }

    /* At this point, the file descriptor table for the process probably
     * looks like this:
     *
     *  File Descriptor #  | Describes
     * --------------------+-------------------
     *  0                  | /dev/stdin
     *  1                  | /dev/stdout
     *  2                  | /dev/stderr
     *  3                  | pipe (read side)
     *  4                  | pipe (write side)
     */

    /* Note: The parent doesn't close either side of the pipe because it
     * will not be using it, but it will be creating two child processes
     * that will use it: 'ls' will write and 'wc' will read.  So, we must
     * keep both ends open in the parent until we have created both child
     * processes or 'ls' won't inherit the write side and 'wc' won't
     * inherit the read side */

    printf("Parent says, \"Doing 'ls -l | wc -l'\"\n");

    /* create child process for 'ls' */
    switch (fork()) {
    case -1:
        fprintf(stderr, "error -- failed to fork()");
        exit(EXIT_FAILURE);

    case 0:
        /* ls is going to be piping its output to wc.
         * Consequently, ls doesn't need the read side of the pipe,
         * so we close it */
        close(fd[READ_SIDE]);

        /* Now, we are going to close stdout file descriptor (1)
         * and duplicate the file descriptor for the write side of the
         * pipe.  We specify that this duplicate should be file
         * descriptor 1 */
        if (dup2(fd[WRITE_SIDE], STDOUT_FILENO) == -1) {
            fprintf(stderr, "error -- dup2() failed for WRITE_SIDE -> STDOUT");
            exit(EXIT_FAILURE);
        }

        /* We don't need file descriptor 4 anymore, close it */
        close(fd[WRITE_SIDE]);

        /* now we replace the address space of the child with 'ls' */
        execlp("ls", "ls", "-l", NULL);
        fprintf(stderr, "error -- exec(ls) failed!");

        /* Now, this child has:
        *
        *  File Descriptor #  | Describes
        * --------------------+-------------------
        *  0                  | /dev/stdin
        *  1                  | pipe (write side)
        *  2                  | /dev/stderr
        */

    default:
        break;
    }

    /*  File Descriptor #  | Describes
     * --------------------+-------------------
     *  0                  | /dev/stdin
     *  1                  | /dev/stdout
     *  2                  | /dev/stderr
     *  3                  | pipe (read side)
     *  4                  | pipe (write side) */

    /* create child process for 'wc' */
    switch (fork()) {
    case -1:
        fprintf(stderr, "error -- failed to fork()");
        exit(EXIT_FAILURE);

    case 0:
        /* ws is going to be reading from the pipe ls is writing to.
         * Consequently, wc doesn't need the write side of the pipe,
         * so we close it */
        close(fd[WRITE_SIDE]);

        /* Now, we are going to close stdin file descriptor (0)
         * and duplicate the file descriptor for the read side of the
         * pipe.  We specify that this duplicate should be file
         * descriptor 0 */
        if (dup2(fd[READ_SIDE], STDIN_FILENO) == -1) {
            fprintf(stderr, "error -- dup2() failed for READ_SIDE -> STDIN");
            exit(EXIT_FAILURE);
        }

        /* We don't need file descriptor 3 anymore, close it */
        close(fd[READ_SIDE]);

        /* now we replace the address space of the child with 'wc' */
        execlp("wc", "wc", "-l", NULL);
        fprintf(stderr, "error -- exec(wc) failed!");

        /* Now, this child has:
        *
        *  File Descriptor #  | Describes
        * --------------------+-------------------
        *  0                  | pipe (read side)
        *  1                  | /dev/stdout
        *  2                  | /dev/stderr
        */

    default:
        break;
    }


    /* parent closes unused file descriptors */
    close(fd[READ_SIDE]);
    close(fd[WRITE_SIDE]);

    /* wait for children, there are 2 and we don't care which order they
     * finish in! */
    wait(NULL);  /* wait(NULL) is the same as waitpid(-1, NULL, 0) */
    wait(NULL);

    return 0;    /* same as exit(EXIT_SUCCESS) */
}
