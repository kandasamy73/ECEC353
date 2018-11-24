/* Example using an unnamed pipe for unidirectional communication from a
 * parent process to a child process.
 *
 *   Author: James A. Shackleford
 *     Date: January 8th, 2015
 *
 * This program starts with the parent process creating an unnamed pipe and
 * then forking a child process.  Because a pipe is a special type of file
 * descriptor, and since child processes inherit file descriptors from their
 * parents, the child process inherits the pipe and is able to use it to
 * communicate with the parent.
 *
 * In this example, the parent process is sending the message "Hello World!"
 * to the child process, so it closes the READ_SIDE of the pipe immediately
 * after forking, since it is not necessary.
 *
 * Likewise, the child process is only reading messages from the pipe, so it
 * closes the WRITE_SIDE of the pipe immediately after the fork(), since it
 * is not necessary.
 *
 * Notice that pipes are handled using the standard read() and write()
 * functions for manipulating file descriptors provided by unistd.h
 *
 * Compile using:
 *   gcc -o 1-pipes 1-pipes.c
 */

#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 25
#define READ_SIDE 0
#define WRITE_SIDE 1

int main(void)
{
    char write_msg[BUFFER_SIZE] = "Hello World!";
    char read_msg[BUFFER_SIZE];
    int fd[2];
    pid_t pid;

    /* create the pipe */
    if (pipe(fd) == -1) {
        fprintf(stderr, "error -- failed to create pipe");
        return 1;
    }

    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "error -- failed to fork()");
        return 1;
    }

    if (pid > 0) { /* parent process */
        int child_ret;

        close(fd[READ_SIDE]);

        write(fd[WRITE_SIDE], write_msg, strlen(write_msg)+1);
        close(fd[WRITE_SIDE]);

        wait(&child_ret);
        printf("parent -- child exited with code %i\n", child_ret);
    } else {       /* child process */
        int ret;
        close(fd[WRITE_SIDE]);

        ret = read(fd[READ_SIDE], read_msg, BUFFER_SIZE);
        if (ret) {
            printf("child -- read [%s] from the pipe.\n", read_msg);
        }
        close(fd[READ_SIDE]);

        exit (1);
    }
}
