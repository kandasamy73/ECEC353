/* A simple example demonstrating how to set the cancelability of a thread
 * using the POSIX threading library pthreads.
 *
 *   Author: James A. Shackleford
 *     Date: January 15th, 2015
 *
 * The cancelability of a thread is defined by two parameters:
 *
 *   -- the cancelation state of the thread
 *        ** disabled
 *        ** enabled       (default)
 *   -- the cancellation type of the thread
 *        ** deferred      (default)
 *        ** asynchronous
 *
 * When a cancellable thread of type ASYNCHRONOUS is sent a cancellation
 * request via pthread_cancel(), the thread will terminate as soon as possible
 * without encountering a cancellation point.  Note that immediate termination
 * of the thread is not guaranteed by the POSIX Thread standard.
 *
 * Compile using: gcc -o 3-cancel-async 3-cancel-async.c -lpthread
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>


void* thread (void* arg)
{
    int i, n;
    int ret;
    int old_type;

    /* Let's set the cancellation state to DISABLE while we do some
     * initialization.  We don't want to get interrupted while we are
     * setting up the thread.  If a cancellation request is sent to the
     * thread at this time, it will be processed once the cancellation
     * state is switched to ENABLED */
    printf("!! Thread -- setting cancellation state: DISABLED\n");
    ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_setcancelstate()\n");
        exit(EXIT_FAILURE);
    }

    /* We will save the old cancellation type so that we can restore it later.
     * Here we want to set the type to asynchronous temporarily while we do
     * some heavy computation -- we want to be able to terminate the
     * computation if necessary. */
    printf("!! Thread -- setting cancellation type: ASYNCHRONOUS\n");
    ret = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &old_type);
    if (ret) {
        fprintf(stderr, "error -- pthread_setcanceltype()\n");
        exit(EXIT_FAILURE);
    }

    printf("!! Thread -- setting cancellation state: ENABLED\n");
    ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_setcancelstate()\n");
        exit(EXIT_FAILURE);
    }

    /* Simulate a highly computationally intensive operation that will take a
     * long time to complete.
     *
     * Here, we use an infinite loop just to demonstrate the fact that the
     * thread was certainly killed asynchronously.  Never use infinite loops
     * in practice! */
    while(1);

    /* This is the end of the region where asynchronous cancellation might be
     * necessary.  Restore the previous cancellation type now that we
     * are done with our work.  */
    ret = pthread_setcanceltype(old_type, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_setcanceltype()\n");
        exit(EXIT_FAILURE);
    }

    printf("!! Thread -- Work Complete! (I shouldn't be able to say this)\n");
    return NULL;
}


int main (int argc, char** argv)
{
    pthread_t tid;
    void* tid_ret;
    int ret;

    ret = pthread_create(&tid, NULL, thread, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_create()\n");
        exit(EXIT_FAILURE);
    }
    printf ("Created new thread (tid: %x)\n", (unsigned int)tid);

    printf ("Sending cancellation request in 5 seconds...\n");
    sleep (5);

    ret = pthread_cancel(tid);
    printf("Sent cancellation request\n");

    if (ret) {
        fprintf(stderr, "error -- pthread_cancel()\n");
        pthread_join(tid, NULL);
        exit(EXIT_FAILURE);
    }

    ret = pthread_join(tid, &tid_ret);
    if (tid_ret == PTHREAD_CANCELED)
        printf ("Thead (tid: %x) successfully cancelled.\n", (unsigned int)tid);
    else
        printf ("Thead (tid: %x) was not cancelled!\n", (unsigned int)tid);

    return 0;
}
