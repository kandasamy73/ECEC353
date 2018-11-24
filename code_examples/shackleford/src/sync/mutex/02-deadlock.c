/* Example demonstrating a simple deadlock as a result of
 * non-standardized mutex locking protocol.
 *
 *   Author: James A. Shackleford
 *     Date: 02/09/2017
 *
 * In the event that multiple mutexes must be locked (i.e. because
 * several shared resources are being used within a single critical
 * section), all programmers on the project must agree on a mutex
 * locking order and stick with it.
 *
 * This example demonstrates what can happen when the mutex locking
 * protocol agreed upon for the project is not adhered to.  Let's assume
 * that the protocol agreed upon for the example file was to always lock
 * foo 1st and bar 2nd if both needed to be locked for the same critical
 * section.
 *
 * Notice how the program will only enter deadlock SOMETIMES (run it a
 * few times to see this)!  This makes catching these types of bugs
 * extremely annoying.
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 500

static int foo;
static pthread_mutex_t mtx_foo = PTHREAD_MUTEX_INITIALIZER;

static int bar;
static pthread_mutex_t mtx_bar = PTHREAD_MUTEX_INITIALIZER;


/* This function DOES NOT comply with the locking protocol */
static void* type_a (void* param)
{
    pthread_mutex_lock(&mtx_foo);
    pthread_mutex_lock(&mtx_bar);
    bar++;
    foo += bar;
    pthread_mutex_unlock(&mtx_bar);
    pthread_mutex_unlock(&mtx_foo);

    return NULL;
}

/* This function complies with the locking protocol */
static void* type_b (void* param)
{
    pthread_mutex_lock(&mtx_foo);
    pthread_mutex_lock(&mtx_bar);
    bar++;
    foo += bar;
    pthread_mutex_unlock(&mtx_bar);
    pthread_mutex_unlock(&mtx_foo);

    return NULL;
}

int main (int argc, char** argv)
{
    int i;
    pthread_t a_threads[NUM_THREADS];
    pthread_t b_threads[NUM_THREADS];

    foo = 10;
    bar = 20;

    for (i=0; i<NUM_THREADS; i++) {
        pthread_create (&a_threads[i], NULL, type_a, NULL);
        pthread_create (&b_threads[i], NULL, type_b, NULL);
    }

    for (i=0; i<NUM_THREADS; i++) {
        pthread_join (a_threads[i], NULL);
        pthread_join (b_threads[i], NULL);
    }

    return EXIT_SUCCESS;
}
