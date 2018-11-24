/* Program demonstrating the removal of a previously created semaphore from the 
 * system. 
 *
 * Author: Naga Kandasamy
 * Date created: July 26, 2018
 *
 * Compile as follows: gcc -o psem_unlink psem_unlink.c -std=c99 -Wall -lpthread
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

int 
main (int argc, char **argv)
{
    if (argc != 2 || strcmp (argv[1], "--help") == 0) {
        printf ("Usage: %s sem-name \n", argv[0]);
        exit (EXIT_FAILURE);
    }

    if (sem_unlink (argv[1]) == -1){
        perror ("sem_unlink");
        exit (EXIT_FAILURE);
    }
    
    exit (EXIT_SUCCESS);
}
