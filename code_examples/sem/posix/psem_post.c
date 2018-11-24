/* Program illustrating the use of the signal function on a named semaphore.
 * The sem_post () function increases the value of a semaphore by 1. 
 *
 * If the value of the semaphore was 0 before the sem_post () call, and some other 
 * process is blocked waiting to decrement the semaphore, then that process is woken up, 
 * and its sem_wait () call proceeds to decrement the semaphore. If multiple processes 
 * are blocked in sem_wait (), then it is indeterminate which one will be woken up 
 * and allowed to decrement the semaphore. 
 *
 * Author: Naga Kandasamy
 * Date created: July 26, 2018
 *
 * Compile as follows: gcc -o psem_post psem_post.c -std=c99 -Wall -lpthread
 * 
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>

int 
main (int argc, char **argv)
{
    sem_t *sem;

    if (argc != 2 || strcmp (argv[1], "--help") == 0){
        printf ("Usage: %s sem-name \n", argv[0]);
        exit (EXIT_FAILURE);
    }

    /* Open the semaphore that was previously created. */
    sem = sem_open (argv[1], 0);
    if (sem == SEM_FAILED) {
        perror ("sem_open");
        exit (EXIT_FAILURE);
    }

    /* Perform the signal operation on the semaphore */
    if (sem_post (sem) == -1) {
        perror ("sem_post");
        exit (EXIT_FAILURE);
    }

    printf ("%ld sem_post () succeeded \n", (long) getpid ());
    exit (EXIT_SUCCESS);
}
