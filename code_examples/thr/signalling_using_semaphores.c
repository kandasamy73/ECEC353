/* Example code that shows how to use semaphores to signal between threads. 
 *
 * Author: Naga Kandasamy
 * Date created: April 5, 2011
 * Date modified: August 20, 2018 
 *
 * Compile as follows: gcc -o signalling_using_semaphores signalling_using_semaphores.c -std=c99 -Wall -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>


/* The data structure shared among all the threads */
typedef struct shared_data_struct_s {
    sem_t semaphore; /* Signals a change to the value */
    pthread_mutex_t mutex; /* Protects access to the value */
    int value; /* The value itself */
} SHARED_DATA;

SHARED_DATA shared_data; 

void *my_thread(void *);

int 
main (int argc, char **argv)
{
    pthread_t tid;
		 
    /* Initialize the semaphore and value within the shared data  */
    shared_data.value = 0;
    sem_init (&shared_data.semaphore, 0, 0); /* Semaphore is not shared among processes and is initialized to 0 */
    pthread_mutex_init (&shared_data.mutex, NULL); /* Initialize the mutex */
	
    /* Create a thread */
    pthread_create (&tid, NULL, my_thread, NULL);
	
    pthread_mutex_lock (&shared_data.mutex);
	
    /* Test the predicate, that is the data and wait for a condition to become true if neccessary */
    if (shared_data.value == 0){
        printf ("Value of data = %d. Waiting for someone to change it and signal me. \n", shared_data.value);
        pthread_mutex_unlock (&shared_data.mutex);
        sem_wait (&shared_data.semaphore); // Probe the semaphore, P(). If the value is zero, then we block, else we decrement the semaphore by one and proceed
    }

    /* Someone changed the value of data to one */
    pthread_mutex_lock (&shared_data.mutex);
    if (shared_data.value != 0) {
        printf ("Change in variable state was signalled. \n");
        printf ("Value of data = %d. \n", shared_data.value);
    }
    pthread_mutex_unlock (&shared_data.mutex);
	
    pthread_join (tid, NULL);
    pthread_exit (NULL);
}

/* The function executed by the thread */
void *
my_thread (void *args)
{
    sleep (5); 
	
    pthread_mutex_lock (&shared_data.mutex);
    shared_data.value = 1; // Change the state of the variable
    pthread_mutex_unlock (&shared_data.mutex);
	
    sem_post (&shared_data.semaphore); // Signal the change to the blocked thread
	
    pthread_exit (NULL);
}
