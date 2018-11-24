/* Example code to illustrate the use of deferred thread cancellation. 
 * 
 * Author: Naga Kandasamy
 * Date created: April 10, 2011
 * Date modified: August 20, 2018
 *
 * Compile as follows: gcc -o deferred_cancellation_v1 deferred_cancellation_v1.c -std=c99 -Wall -lpthread
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

static int counter;

void *
my_thread (void *args)
{
    /* Stay in an infinite loop and check for a cancellation request from the creator every 1000 iterations */
    for (counter = 0; ; counter++) {
        if ((counter % 1000) == 0) {
            /* printf("Thread %d is checking for cancellation request. \n", pthread_self()); */
            pthread_testcancel ();
        }
    }
} 

int 
main (int argc, char **argv)
{
    pthread_t tid;
    void *result;
	
    /* Create the thread. The thread is always created with its cancellation flag set to 
     * DEFERRED as the default setting. */
    printf ("Creating the thread. \n");
    pthread_create (&tid, NULL, my_thread, NULL);

    /* Simulate some processing. */
    sleep (2); 
    
    printf ("Cancelling the thread. \n");
    pthread_cancel (tid);

    pthread_join (tid, &result);
    
    if (result == PTHREAD_CANCELED)
        printf ("Thread was cancelled at iteration %d. \n", counter);
    else
        printf ("Thread was not cancelled. \n");
		
    pthread_exit (NULL);
}

