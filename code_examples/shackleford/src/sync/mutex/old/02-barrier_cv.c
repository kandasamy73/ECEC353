/* Example of using condition variables to implement a barrier
 *
 *   Author: James A. Shackleford
 *     Date: 01/20/2015
 *
 * This example shows how to use condition variables with mutexes to
 * implement a barrier.
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

#define NUM_THREADS 5
#define NUM_ITERATIONS 5

/* Define the barrier data structure */
struct barrier {
    int counter;

    pthread_mutex_t mutex;
    pthread_cond_t condition;
};

/* Create & initialize a barrier for ourselves named my_barrier */
static struct barrier my_barrier = {
    .counter   = 0,
    .mutex     = PTHREAD_MUTEX_INITIALIZER,
    .condition = PTHREAD_COND_INITIALIZER
};


/* Utility function returning a random number between 0 and max */
unsigned int random_uint(unsigned int max)
{
    float tmp = (float)rand()/(float)RAND_MAX * (float)max;
    return (unsigned int)ceilf(tmp);
}


/* This function implements a barrier.  Threads calling this
 * function will block until ALL threads have called it. */
void barrier_sync (struct barrier* b)
{
    pthread_mutex_lock(&b->mutex);

    b->counter++;

    /* Did all threads reach the barrier? */
    if (b->counter == NUM_THREADS) {
        b->counter = 0;
        /* Wake up all threads waiting on condition variable */
        pthread_cond_broadcast(&b->condition);
    } else {
        /* Not all threads have hit the barrier yet, so we put
         * this thread to sleep.  When the last thread hits the
         * barrier, it will send a signal to all threads via
         * the condition variable. */

        /* Note, pthread_cond_wait():
         *  -- Unlocks the mutex
         *  -- Blocks until the condition variable is signaled
         *  -- Relocks the mutex
         *  -- Returns
         */
        pthread_cond_wait(&b->condition, &b->mutex);
    }

    pthread_mutex_unlock(&b->mutex);
}


void* thread (void* param)
{
    unsigned int i;
    unsigned int* thread_num = (unsigned int*)param;

    for (i=0; i<NUM_ITERATIONS; i++) {
        printf("Thread #%u -- iteration %u\n", *thread_num, i);

        /* Simulate a random amount of work taking between 0 to 10 seconds */
        sleep(random_uint(10));

        printf("Thread #%u -- encountered barrier\n", *thread_num);
        barrier_sync(&my_barrier);
    }

    pthread_exit(0);
}


int main (int argc, char** argv)
{
    unsigned int i;
    pthread_t thread_id[NUM_THREADS];
    unsigned int thread_num[NUM_THREADS];

    for (i=0; i<NUM_THREADS; i++) {
        thread_num[i] = i;
        pthread_create(&thread_id[i], NULL, thread, &thread_num[i]);
    }

    for (i=0; i<NUM_THREADS; i++) {
        pthread_join(thread_id[i], NULL);
    }

    return 0;
}
