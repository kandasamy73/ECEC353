/* Example demonstrating the need for condition variables in a producer
 * consumer problem.
 *
 *   Author: James A. Shackleford
 *     Date: 01/20/2015
 *  Updated: 02/22/2018
 *
 * This example DOES NOT use condition variables, but shows why we need
 * them.  Here, notice how the consumer thread must continuously check
 * the variable avail.  This is known as "spinning" on a variable and is
 * a waste of CPU cycles.  This can be fixed using a condition variable.
 * See next example.
 *
 * Note: This code does not perform error checking in order to be more
 * readable.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

#define NUM_PRODUCERS 10000

#define READ_SIDE 0
#define WRITE_SIDE 1


struct shared_data {
    unsigned int avail;
    unsigned int producers;
    int work_pipe[2];
    pthread_mutex_t mtx;
};


/* Used by threads to communicate how much work has been produced by the
 * producer thread that still needs to be consumed by the consumer
 * thread.  Since both threads will need to read/write this variable, we
 * need to protect it with a mutex.
 *
 * Also, we don't care about the return values of producer threads, so
 * we are going to detach them all.  So, we will use the variable
 * `producers` to keep track of how many are still active.  Once it
 * reaches zero, the consumer thread will know that no more work units
 * will be produced.  At this point, the comsumer thread will terminate.
 */
static struct shared_data sdata = {
    .avail = 0,
    .producers = NUM_PRODUCERS,
    .mtx = PTHREAD_MUTEX_INITIALIZER
};


/* Utility function returning a random number between 0 and max */
static unsigned int random_uint(unsigned int max)
{
    float tmp = (float)rand()/(float)RAND_MAX * (float)max;
    return (unsigned int)ceilf(tmp);
}


static void* producer (void* param)
{
    int ret;
    unsigned int* thread_num = (unsigned int*)param;
    unsigned int result;

    /* < Produce some work > */
    /*   Here every thread is simply going to produce its own thread_num
     *   as the work unit.  In practice, this would be the result of a
     *   more complex operation. */
    result = *thread_num;

    do {
        sleep(random_uint(2));
        pthread_mutex_lock(&sdata.mtx);
        ret = write(sdata.work_pipe[WRITE_SIDE], &result, sizeof(result));
        if (ret <= 0) {
            /* pipe is full, try again later */
            fprintf (stderr, "producer -- pipe was full!\n");
            pthread_mutex_unlock(&sdata.mtx);
            continue; 
        }
        sdata.avail++;
        sdata.producers--;

        pthread_mutex_unlock(&sdata.mtx);
    } while (0);
}


void main (int argc, char** argv)
{
    unsigned int i, ret;
    unsigned int work_unit;
    unsigned int finished = 0;
    pthread_t thread_id[NUM_PRODUCERS];
    unsigned int thread_num[NUM_PRODUCERS];
    unsigned int processed = 0;

    /* Each producer will produce a work unit that will be processed by
     * the consumer thread.  In this example, each producer will simply
     * produce a result value and the consumer will keep a running sum
     * of these result values as they are produced.*/
    int sum = 0;

    /* Create a pipe for producers to use for sending results to the
     * consumer thread */
    pipe(sdata.work_pipe);

    /* Make the pipe non-blocking.  We access the pipe from within
     * critical sections.  If a thread blocks on the pipe while it has
     * the mutex, then we will be deadlocked because it will be
     * impossible for another thread to clear the pipe without being
     * able to obtain the mutex. */
    fcntl(sdata.work_pipe[READ_SIDE], F_SETFL, O_NONBLOCK);
    fcntl(sdata.work_pipe[WRITE_SIDE], F_SETFL, O_NONBLOCK);

    /* Create the producer threads */
    for (i=0; i<NUM_PRODUCERS; i++) {
        thread_num[i] = i;
        pthread_create(&thread_id[i], NULL, producer, &thread_num[i]);
        pthread_detach(thread_id[i]);
    }

    /* Now, the main thread will act as the consumer thread.
     * Once a producer has generated a result, the consumer will
     * will process it. */
    while (!finished) {
        /* check if there is work to do that has been
         * produced by the producer thread */
        processed = 0;
        pthread_mutex_lock(&sdata.mtx);
        while (sdata.avail > 0) {
            /* < Do something with work unit > */
            /*   Get the next work unit and add it to the sum */
            ret = read(sdata.work_pipe[READ_SIDE], &work_unit, sizeof(work_unit));
            if (ret > 0) {
                sum += work_unit;
                sdata.avail--;
                processed++;
            } else {
                /* pipe was empty, this should not happen! */
                fprintf (stderr, "consumer -- tried to read from empty pipe!\n");
            }

            /* check to see if there are still active producer threads */
            if (!sdata.producers)
                finished = 1;
        }
        pthread_mutex_unlock(&sdata.mtx);

        if (processed)
            printf ("consumer -- processed %u work units\n", processed);
    } /* while (!finished) */

    /* The actual answer */
    printf ("  Work unit sum: %u\n", sum);

    /* Compute what the answer should have been using a single thread */
    sum = 0;
    for (i=0; i<NUM_PRODUCERS; i++)
        sum += i;

    printf ("Expected result: %u\n", sum);
}
