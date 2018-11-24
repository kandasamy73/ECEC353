/* Example code that shows how to use semaphores to implement a barrier.
 * 
 * Author: Naga Kandasamy
 * Date created: April 5, 2011
 * Date modified: August 27, 2018
 *
 * Compile as follows: 
 * gcc -o barrier_synchronization_with_semaphores barrier_synchronization_with_semaphores.c -std=c99 -Wall -lpthread -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>

#define NUM_THREADS 5
#define NUM_ITERATIONS 3

typedef struct barrier_struct {
    sem_t counter_sem; /* Protects access to the counter */
    sem_t barrier_sem; /*Signals that barrier is safe to cross */
    int counter; /* The counter value */
} BARRIER;

/* Create the barrier data structure */
BARRIER barrier;  

/* Function prototypes */
void *my_thread (void *);
void barrier_sync (BARRIER *, int);

int num_threads, num_iterations;

int 
main (int argc, char **argv)
{

    /* Get command line arguments */
    if (argc != 3) {
        printf ("Usage: %s num-threads num-iterations \n", argv[0]);
        exit (EXIT_FAILURE);
    }

    num_threads = atoi (argv[1]);
    num_iterations = atoi (argv[2]);

    pthread_t *thread_id = (pthread_t *) malloc (sizeof (pthread_t) * num_threads);
    int i;
		  
    /* Initialize the barrier data structure */
    barrier.counter = 0;
    sem_init (&barrier.counter_sem, 0, 1); /* Initialize the semaphore protecting the counter to 1 */
    sem_init (&barrier.barrier_sem, 0, 0); /* Initialize the semaphore protecting the barrier to 0 */

    /* Create the threads */
    for (i = 0; i < num_threads; i++) 
        pthread_create (&thread_id[i], NULL, my_thread, (void *)i);

    /* Wait to reap the threads that we have created */
    for (i = 0; i < num_threads; i++)
        pthread_join (thread_id[i], NULL);
		  
    pthread_exit (NULL);
}

/* The function executed by the threads. */
void *
my_thread (void *args)
{
    int thread_number = (int) args;
    int i;

    for (i = 0; i < num_iterations; i++) {
        printf ("Thread number %d is processing for iteration %d. \n", thread_number, i);
        sleep (ceil ((float) rand()/(float) RAND_MAX * 10)); /* Simulate processing */

        printf ("Thread %d is at the barrier. \n", thread_number);
        barrier_sync (&barrier, thread_number); /* Wait here for all threads to catch up before starting the next iteration */
    }

    pthread_exit (NULL);
}

/* The function that implements the barrier synchronization. */
void 
barrier_sync (BARRIER *barrier, int thread_number)
{
    sem_wait (&(barrier->counter_sem)); /* Obtain the lock on the counter */

    /* Check if all threads before us, that is NUM_THREADS-1 threads have reached this point */
    if (barrier->counter == (num_threads - 1)) {
        barrier->counter = 0; /* Reset the counter */
					 
        sem_post (&(barrier->counter_sem)); 
					 
        /* Signal the blocked threads that it is now safe to cross the barrier */			 
        printf("Thread number %d is signalling other threads to proceed. \n", thread_number); 			 
        for (int i = 0; i < (num_threads - 1); i++)
            sem_post (&(barrier->barrier_sem));
    } 
    else {
        barrier->counter++; // Increment the counter
        sem_post (&(barrier->counter_sem)); // Release the lock on the counter
        sem_wait (&(barrier->barrier_sem)); // Block on the barrier semaphore and wait for someone to signal us when it is safe to cross
    }
}



