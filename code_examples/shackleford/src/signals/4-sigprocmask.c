/* This simple example demonstrates how to create signal sets and use them
 * to define which signals will be marked in the process's signal mask.
 *
 * Signals set in a process's signal mask will not be caught unless the
 * signal mask is later modified to unset the signal in question.
 * Masked signals sent to a process will be pending and will be caught
 * once the process has unmasked the signal and gets swapped in by
 * scheduler.  Keep in mind that if a signal is send to a process
 * multiple times while the signal is masked, the signal will only be
 * sent once when the signal is later unmasked by the process.  This
 * behavior is what is meant when people say that masked signals are
 * "pending" and not "queued".
 *
 *   Author: James A. Shackleford
 *   Date: February 18th, 2016
 */

#include <limits.h>
#include <signal.h>
#include <stdio.h>

unsigned long int do_calculation()
{
    unsigned int i;
    unsigned long int acc;

    acc = 0;
    for (i=0; i<UINT_MAX; i++) {
        acc += i;
    }

    return acc;
}


int main (int argc, char** argv)
{
    unsigned long int result;

    sigset_t block;
    sigset_t prev_mask;

    /* Initialize the signal set to be all unset */
    sigemptyset(&block);

    /* Set the signal in the set corresponding to SIGINT */
    sigaddset(&block, SIGINT);

    /* Let's modify the the signal mask.  Signals set in the
    * signal mask will be ignored by the process.
    *
    * The first parameter is the *how* parameter.
    * This can be:
    *    -- SIG_BLOCK:   add set signals to the signal mask
    *    -- SIG_UNBLOCK: remove set signals from the signal mask
    *    -- SIG_SETMASK: use the supplied set as the signal mask
    */
    sigprocmask(SIG_BLOCK, &block, &prev_mask);

    printf("Starting important calculation... ");
    fflush(stdout);

    /* This "important calculation" cannot be interrupted by SIGINT */
    result = do_calculation();
    printf ("done. (result: %lu)\n", result);

    /* Restore the old signal mask */
    sigprocmask(SIG_SETMASK, &prev_mask, NULL);

    printf("Starting UNimportant calculation... ");
    fflush(stdout);

    /* This "UNimportant calculation" *CAN* be interrupted by SIGINT */
    result = do_calculation();
    printf ("done. (result: %lu)\n", result);

    return 0;
}
