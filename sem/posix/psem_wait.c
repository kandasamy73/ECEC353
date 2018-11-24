/* Program illustrating the use of the probe function on a named semaphore.
 * The probe function decrements the value of a semaphore. If the semaphore 
 * value is greater than 0, the sem_wait() function returns immediately; if the value 
 * is currently 0, sem_wait() blocks until the semaphore value rises above 0; at that 
 * time, the semaphore is then decremented and sem_wait () returns.
 *
 * Author: Naga Kandasamy
 * Date created: July 26, 2018
 *
 * Compile as follows: gcc -o psem_wait psem_wait.c -std=c99 -Wall -lpthread
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

    /* Perform the probe operation on the semaphore */
    if (sem_wait (sem) == -1) {
        perror ("sem_wait");
        exit (EXIT_FAILURE);
    }

    printf ("%ld sem_wait () succeeded \n", (long) getpid ());
    exit (EXIT_SUCCESS);
}
