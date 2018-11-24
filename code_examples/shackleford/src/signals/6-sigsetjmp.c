/* This is a simple example that shows how to use setjmp()/longjmp()
 * safely from within a signal handler.  This is accomplished by using
 * the special versions of these functions: sigsetjmp()/siglongjmp().
 *
 * If you are unfamilier with setjmp()/longjmp(), it is worth reading
 * about.  They are commonly referred to as "non-local gotos" and
 * should be used with caution as they are quite powerful.
 *
 *   Author: James A. Shackleford
 *     Date: February 18th, 2016
 */

#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CALC_SIZE 30

#define NORMAL 1984
#define CTRLC  1337

#define FALSE 0
#define TRUE !FALSE

/* used to store the stack context/environment */
static sigjmp_buf env;


void handler(int sig)
{
    /* Recall: when a signal handler is invoked for a particular
     *         signal, the kernel masks that signal while the
     *         handler is executing.  Therefore, if we don't restore
     *         the signal mask before siglongjmp()ing back, the
     *         SIGINT signal would continue to be masked, which would
     *         prevent subsequent catching of SIGINT after the first
     *         siglongjmp().
     *
     *         This is why we use sigsetjmp() and siglongjmp() instead
     *         of setjmp() longjmp() inside of signal handlers.
     */

    /* siglongjmp() sets the program counter back to the last call of
     * sigsetjmp().  Wherever that was, sigsetjmp() will appear to return
     * as if it were just called, however, its return value will be the
     * value specifed in the 2nd parameter of siglongjmp(), which we use to
     * identify that we are returning from a siglongjmp().  If the 2nd
     * parameter of sigsetjmp() was non-zero (TRUE in this case), then the
     * signal mask will be restored by siglongjmp(), thereby allowing
     * subsequent SIGINT signals to be caught.
     */
    siglongjmp(env, CTRLC);
}


unsigned long int do_calculation(int* buf, int num)
{
    unsigned int i;

    for (i=0; i<num; i++) {
        buf[i] = i*i;
        sleep(1);
    }
}


int main (int argc, char** argv)
{
    unsigned int i;
    unsigned long int total;
    int ret;

    unsigned int* result_buffer;

    result_buffer = malloc(CALC_SIZE * sizeof(unsigned int));
    memset (result_buffer, 0, CALC_SIZE*sizeof(*result_buffer));

    (void)signal(SIGINT, handler);
    /* Note: explicitly casting the return value of signal() to void
     *       is just a way of telling other programmers who read our
     *       code that we are intentionally ignoring the return value
     *       (i.e. address of the old signal handler), this doesn't
     *       actually _do_ anything */

    /* sigsetjmp() returns 0 when called directly and non-zero when
     * returning from a siglongjmp().  Supplying a non-zero 2nd paramter
     * to sigsetjmp() means that subsequent siglongjmp() calls from inside
     * a signal handler will restore the signal mask
     */
    ret = sigsetjmp(env, TRUE);
    switch (ret) {
        case 0:
            /* returned from explicit setsetjmp() call */
            break;
        case CTRLC:
            printf("\nReturned from longjmp!\n");
            printf("Cleaning up and exiting safely...\n");
        case NORMAL:
            for (i=0, total=0; i<CALC_SIZE; i++) {
                total += result_buffer[i];
            }
            printf ("total: %lu\n", total);
            free(result_buffer);
            exit(EXIT_SUCCESS);
    }

    /* Perform a completely convoluted calculation */
    do_calculation(result_buffer, CALC_SIZE);

    siglongjmp(env, NORMAL);

    return 0;
}
