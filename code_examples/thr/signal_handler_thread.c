/*   This code illustrates how to create a signal handler thread.
 *
 * Author: Naga Kandasamy
 * Date created: January 22, 2009
 * Date modified: August 20, 2018

 * Compile as follows: gcc -o signal_handler_thread signal_handler_thread.c -std=c99 -Wall -lpthread 
*/

#define _POSIX_C_SOURCE 1

#define _REENTRANT
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void *signal_handler(void *); /* The signal handler prototype */


int 
main (int argc, char **argv)
{
    sigset_t set; /* Define the set of signals */
    pthread_t tid;
  
    /* Block all signals in the main thread. Threads created after this will 
     * inherit this, i.e, also block all signals 
     * */
  
    sigfillset (&set);
    sigprocmask (SIG_BLOCK, &set, NULL);
  
    /* Create a signal_handler thread to handle all non-directed signals */
    if ((pthread_create (&tid, NULL, signal_handler, NULL)) != 0){
        perror ("pthread_create");
        exit (EXIT_FAILURE);
    }
  
    printf ("Main thread processing \n");
  
    while (1) {
        /* Simulate some processing in the main thread. */
    }
  
    exit (EXIT_SUCCESS);
}

/* The signal handler thread. */
void *
signal_handler (void *arg)
{
    sigset_t set;
    int sig;
  
    sigfillset(&set); /* Catch all signals */
  
    while (1) {
        /* Wait for some signal to arrive and handle the signal appropriately. */
        if (sigwait (&set, &sig) != 0) {
            perror ("sigwait");
            break;
        }
			
        switch (sig) {
            case SIGINT: 
                printf ("Ctrl+C received ... quiting \n");
                exit (EXIT_SUCCESS);
				
            case SIGUSR1: 
                printf ("Received SIGUSR1 signal \n");
                break;
				
            case SIGUSR2:
                printf ("Received SIGUSR2 signal \n");
                break;
				
            default:
                printf ("Received some signal \n");
			
        }
  
    }
  
    return ((void *)0);
}
