/* This program demonstrates how to install a SIGCHLD signal handler
 * that will wait() on terminated child processes in direct response to
 * their termination.  This removes the need for the parent to perform
 * one of the only 2 other alternatives:
 *
 *    1. Dedicate a thread to blocking on wait() for children
 *    2. Dedicate a thread to spinning on a waidpid(pid[i], &ret, WNOHANG)
 *       loop, which unnecessarily wastes valuable CPU cycles.

 *   Author: James A. Shackleford
 *     Date: January 31st, 2018
 *
 * Compile using:
 *   gcc -o 5-sigchld 5-sigchld.c
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>


void handler (int sig)
{
    pid_t chld;

    fprintf (stderr, "Entered handler()\n");

    /* Remember that while in a signal handler, the signal that
     * brought us here is masked and does not queue.  So, it is possible
     * that multiple children may exit() while we are in the handler.
     * In other words, the invocation of this signal handler is not
     * 1:1 for every terminated child process. So, while we are in here
     * we will keep wait()ing on zombie children until there are no
     * more. */
    while ((chld = waitpid(-1, NULL, WNOHANG)) > 0) {
        printf ("wait()ed on child pid: %i\n", chld);
        fflush(stdout);
        continue;
    }

    /* Recall:
     * the WNOHANG flag makes the call to waitpid() non-blocking */
}


int main (int argc, char** argv)
{
    int i;
    pid_t pid;

    /* A parent receives SIGCHLD from the kernel whenever one
     * of its children has terminated (or is stopped or continued).
     * Here we set the disposition for SIGCHLD to a handler that will
     * wait() on these children in response to this signal. */
    signal(SIGCHLD, handler);

    for (i=0; i<5; i++) {
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "error -- failed to fork()\n");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            exit(EXIT_SUCCESS);
        }
    }

    printf ("Just fork()ed %i processes! Counting to 12 now!\n", i);

    for (i=0; i<=12; i++) {
        printf("%i\n", i);
        fflush(stdout);
        sleep(1);
    }

    return 0;
}
