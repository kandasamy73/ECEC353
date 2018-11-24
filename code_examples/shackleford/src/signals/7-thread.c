/* A simple example demonstrating how to create a thread that handles signals
 * using the POSIX threading library pthreads.
 *
 *   Author: James A. Shackleford
 *     Date: January 15th, 2015
 *
 * Signals are a simple mechanism for threads and processes to communication
 * that certain notable events have occurred.  For a list of signals, please
 * consult signals.pdf on BBLearn.
 *
 * You can send signals from the commandline using the kill command:
 *
 *   $ kill -s SIGUSR1 <process id>
 *   $ kill -s SIGUSR2 <process id>
 *   $ kill -s SIGINT  <process id>
 *
 * Compile using:
 *   gcc -o 5-signal_handler 5-signal_handler.c -lpthread
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

void* signal_handler (void* arg)
{
    sigset_t set;
    int sig;
    int ret;

    sigfillset(&set);

    while (1) {
        /* Catch all signals */
        ret = sigwait(&set, &sig);
        if (ret) {
            printf("error -- sigwait()\n");
            exit(EXIT_FAILURE);
        }

        switch (sig) {
            case SIGINT:
                printf("signal_handler -- caught Ctrl+C -- terminating\n");
                exit(0);
            case SIGUSR1:
                printf("signal_handler -- caught SIGUSR1\n");
                break;
            case SIGUSR2:
                printf("signal_handler -- caught SIGUSR2\n");
                break;
            default:
                printf("signal_handler -- caught some other signal\n");
        }
    }

    return NULL;
}


int main (int argc, char** argv)
{
    pthread_t tid;
    sigset_t set;
    int ret;

    /* Create a full signal mask as use it to block all signals sent to the
     * process.  All threads created will inherit this signal table.
     */
    sigfillset(&set);
    sigprocmask(SIG_BLOCK, &set, NULL);

    /* Spawn the thread that will do our signal handling */
    ret = pthread_create(&tid, NULL, signal_handler, NULL);
    if (ret) {
        printf("error -- pthread_create()\n");
        exit(EXIT_FAILURE);
    }

    printf("Doing a ton of work...\n");
    while(1);

    return 0;
}
