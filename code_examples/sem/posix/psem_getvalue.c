/* Program returns the current value of the semaphore specified in the 
 * command line. 
 *
 * Author: Naga Kandasamy
 * Date: July 26, 2018
 *
 * Compile as follows: gcc -o psem_getvalue psem_getvalue.c -std=c99 -Wall -lpthread
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
    int value;

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

    /* Obtain the current value of the semaphore */
    if (sem_getvalue (sem, &value) == -1) {
        perror ("sem_getvalue");
        exit (EXIT_FAILURE);
    }

    printf ("Semaphore value = %d \n", value);
    exit (EXIT_SUCCESS);
}

