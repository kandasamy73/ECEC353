/* Example code that shows how to use condition variables to implement a barrier.
 * 
 * Author: Naga Kandasamy
 * Date created: April 5, 2011
 * Date modifeid: Agust 27, 2018
 *
 * Compile as follows: 
 * barrier_synchronization_with_condition_variables barrier_synchronization_with_condition_variables.c -std=c99 -Wall -lpthread -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>

#define NUM_THREADS 5
#define NUM_ITERATIONS 5

typedef struct barrier_struct {
    pthread_mutex_t mutex; /* Protects access to the value */
    pthread_cond_t condition; /* Signals a change to the value */
    int counter; /* The counter value */
} BARRIER;

/* Create the barrier data structure and initialize it */
BARRIER barrier = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0}; 

/* Function prototypes */
void *my_thread (void *);
void barrier_sync (BARRIER *);

/* Global variables */
int num_threads, num_iterations;

int 
main (int argc, char **argv)
{
    if (argc != 3) {
        printf ("Usage: %s num-threads num-iterations \n", argv[0]);
        exit (EXIT_FAILURE);
    }

    num_threads = atoi (argv[1]);
    num_iterations = atoi (argv[2]);

    pthread_t *thread_id = (pthread_t *) malloc (sizeof(pthread_t) * num_threads);
    int i;

    /* Create the threads */
    for (i = 0; i < num_threads; i++)
        pthread_create (&thread_id[i], NULL, my_thread, (void *) i);
		  
    /* Wait to reap the threads that we have created */
    for (i = 0; i < num_threads; i++)
        pthread_join (thread_id[i], NULL);

    pthread_exit (NULL);
}

/* The function executed by the threads. */
void *
my_thread (void *args)
{
    int thread_number = (int)args;
    int i;
	
    for (i = 0; i < num_iterations; i++) {
        printf ("Thread number %d is processing for iteration %d. \n", thread_number, i);
		
        sleep (ceil ((float) rand()/(float) RAND_MAX * 10)); /* Simulate processing */

        printf ("Thread %d is at the barrier. \n", thread_number);
					 
        barrier_sync (&barrier); // Wait here for all threads to catch up before starting the next iteration
    }

    pthread_exit (NULL);
}

/* The function that implements the barrier synchronization. */
void 
barrier_sync (BARRIER *barrier)
{
    pthread_mutex_lock(&(barrier->mutex));
    barrier->counter++;
    
    /* Check if all threads have reached this point */
    if (barrier->counter == num_threads) {
        barrier->counter = 0; /* Reset the counter */
        pthread_cond_broadcast (&(barrier->condition)); /* Signal this condition to all the blocked threads */
    } 
    else 
        while ((pthread_cond_wait (&(barrier->condition), &(barrier->mutex))) != 0); /* We may be woken up by events other than a broadcast. If so, we go back to sleep */
    
    pthread_mutex_unlock (&(barrier->mutex));
}



