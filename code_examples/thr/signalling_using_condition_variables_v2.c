/* Example code that shows how to use condition variables to signal between threads. This code shows how to broadcast a condition variable. 
 * 
 * Author: Naga Kandasamy
 * Date created: April 5, 2011
 * Date modifeid: August 27, 2018
 *
 * Compile as follows: 
 * gcc -o signalling_using_condition_variables_v2 signalling_using_condition_variables_v2.c -std=c99 -Wall -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct my_struct_s {
    pthread_mutex_t mutex; /* Protects access to the value */
    pthread_cond_t condition; /* Signals a change to the value */
    int value; /* The value itself */
} MY_STRUCT;

/* Create the data structure and initialize it */
MY_STRUCT data = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0}; 

/* Function prototypes */
void *waiting_thread (void *);
void *signalling_thread (void *);

int 
main (int argc, char **argv)
{
    
    /* Check command line arguments */
    if (argc != 2) {
        printf ("Usage: %s num-threads \n", argv[0]);
        exit (EXIT_FAILURE);
    }

    int num_threads = atoi (argv[1]);

    /* Data structure to store the IDs of the waiting threads */
    pthread_t *tid;
    tid = malloc (sizeof (pthread_t) * num_threads);
    if (tid == NULL) {
        perror ("malloc");
        exit (EXIT_FAILURE);
    }

    /* Data type to store the ID of the signal handling thread */
    pthread_t signal_tid;
    int i;
		  
    /* Create the waiting threads */
    for (i = 0; i < num_threads; i++)
        pthread_create (&tid[i], NULL, waiting_thread, (void *)i);
		  
    /* Create the signalling thread. */
    pthread_create (&signal_tid, NULL, signalling_thread, NULL);

    /* Wait to reap the threads that we have created */
    pthread_join (signal_tid, NULL);
    for (i = 0; i < num_threads; i++)
        pthread_join (tid[i], NULL);

    pthread_exit (NULL);
}

/* The function executed by the waiting threads. */
void *
waiting_thread (void *args)
{
    int thread_number = (int) args;

    /* Check the predicate */
    pthread_mutex_lock (&data.mutex); 
    if (data.value == 0) {
        printf ("The value of data is %d. \n", data.value);
        printf ("Thread number %d waiting for the condition to be signalled. \n", thread_number);
        pthread_cond_wait (&data.condition, &data.mutex); /* Release mutex and block here for condition to be signalled */
    }

    /* The cond_wait signal will unblock with the corresponding mutex locked */
    if (data.value != 0) {
        printf ("Condition signalled to thread number %d. \n", thread_number);
        printf ("The value of data = %d. \n", data.value);
    }
    pthread_mutex_unlock (&data.mutex);
	
    pthread_exit (NULL);
}


/* The function executed by the signalling thread */
void *
signalling_thread (void *args)
{
    int status;
	
    sleep (5); /* Simulate some processing */
	
    status = pthread_mutex_lock (&data.mutex);
    if (status != 0) {
        perror ("pthread_mutex_lock error ");
        pthread_exit (NULL);
    }

    data.value = 1; /* Change the state of the variable */
    status = pthread_cond_broadcast (&data.condition); /* Signal the change in state to all the waiting threads */
    if (status != 0) {
        perror ("pthread_cond_broadcast error ");
        pthread_exit (NULL);
    }

    status = pthread_mutex_unlock (&data.mutex);
    if (status != 0) {
        perror ("pthread_mutex_unlock error ");
        pthread_exit (NULL);
    }

    pthread_exit (NULL);
}
