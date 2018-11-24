/* This simple example demonstrates how to install a signal handler
 * with the intention of catching the SIGINT and SIGQUIT signals.
 *
 * Restoring of the prior/default signal handlers is also demonstrated.
 *
 *   Author: James A. Shackleford
 *     Date: February 18th, 2016
 */

#include <signal.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

unsigned int i;
unsigned long int acc;

void custom_handler(int sig)
{
    /* Note: printf() is not reentrant and should not be used in
     *       a signal handler.  It is only used here for the sake
     *       of easily demonstrating signal handlers.  Production
     *       code should never do this.  Instead, the signal handler
     *       should set a flag on the heap or data segment that is
     *       checked periodically outside of the signal handler.
     */

    if (sig == SIGINT) {
        printf("\nCaught Signal: SIGINT\n");
        printf("Current Progress: [i: %u, acc: %lu]\n", i, acc);
        printf("Exiting Gracefully...\n\n");
        exit(EXIT_SUCCESS);
    } else if (sig == SIGQUIT) {
        printf("\nCaught Signal: SIGQUIT\n");
        printf("Exiting Gracefully...\n\n");
        exit(EXIT_SUCCESS);
    }
}

int main (int argc, char** argv)
{
    /* Function pointer for storing old SIGINT handler */
    void (*old_sigint_handler)(int sig);
    void (*old_sigquit_handler)(int sig);

    /* Let's change the SIGINT disposition to something that will
     * report the progress of our computation if the process is
     * interrupted before successfully completing.  While we are at
     * it, let's also change the disposition of SIGQUIT to something
     * more friendly.  Both signals will be caught by the same handler.
     */

    old_sigint_handler = signal(SIGINT, custom_handler);
    old_sigquit_handler = signal(SIGQUIT, custom_handler);

    acc = 0;
    for(i=0; i<UINT_MAX; i++)
        acc += i;

    /* Done with computation.  Restore old signal handlers. */
    signal(SIGINT, old_sigint_handler);
    signal(SIGQUIT, old_sigquit_handler);

    printf("Successfully completed: %lu\n", acc);

    return 0;
}
