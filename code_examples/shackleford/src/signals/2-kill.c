/* This simple example demonstrates how to send a signal from one
 * process to another.
 *
 * Specifically, this example:
 *   -- creates a child process
 *   -- installs a SIGINT signal handler for the child
 *   -- sends SIGINT to the child from the parent using kill()
 *   -- reaps the child using waitpid()
 *
 *   Author: James A. Shackleford
 *     Date: February 18th, 2016
 */

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void child_handler(int sig)
{
    if (sig == SIGINT) {
        printf("\nChild: Caught SIGINT!\n\nExiting..\n\n");
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char** argv)
{
    pid_t pid;
    void (*old_handler)(int sig);
    int ret;

    pid = fork();
    if (pid == 0) {
        old_handler = signal(SIGINT, child_handler);
        while(1);
    }

    printf("Parent: Sleeping for 5 seconds...\n");
    sleep(5);

    printf("Parent: Sending SIGINT to child in 1 second...\n");
    sleep(1);
    kill(pid, SIGINT);

    waitpid(pid, NULL, 0);

    return 0;
}
