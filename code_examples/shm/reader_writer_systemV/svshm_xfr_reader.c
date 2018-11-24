/* The reader program that transfers data written by the writer to a shared memory area 
 * and displays it on stdout. Once the data has been transferred from shared memory, the writer 
 * process is signalled via a semaphore.  
 *
 * Author: Naga Kandasamy
 * Date modified: July 17, 2018
 *
 * Notes: Code taken from: Michael Kerrisk, The Linux Programming Interface, No Starch Press, 2010.
 *
 * Compile as follows: make clean && make
 *
 */

#define _SVID_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "semun.h"
#include "svshm_xfr.h"

void			/* Print out the error returned by the system and exit. */ 
errExit (char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}


int
main (int argc, char **argv)
{
    int semid, shmid, nbytes, xfrs;
    struct shmseg *shmp;
	
    /* Get the ids for the semaphore set and shared memory previouslt created by the writer. */
    semid = semget (SEM_KEY, 0, 0);
	
    if(semid == -1)
        errExit ("semget");
		
    shmid = shmget (SHM_KEY, 0, 0);
    if(shmid == -1)
        errExit ("shmget");
		
    shmp = shmat (shmid, NULL, SHM_RDONLY);
    if(shmp == (void *) -1)
        errExit ("shmat");

    /* Transfer blocks of data from shared memory to stdout. */
    for (nbytes = 0, xfrs = 0; ; xfrs++) {
	
        if (reserveSem (semid, READ_SEM) == -1)		/* Wait for our turn. */
            errExit ("reserveSem");
			
        if (shmp->count == 0)								/* Writer encountered EOF. */
            break;
			
        nbytes += shmp->count;
		
        if(write (STDOUT_FILENO, shmp->buf, shmp->count) != shmp->count)
            errExit ("write");
					
        if (releaseSem (semid, WRITE_SEM) == -1)								/* Give the writer a turn. */
            errExit ("releaseSem");	
    }
		   
    if (shmdt (shmp) == -1)
        errExit ("shmdt");
		
    /* Give the writer a turn so that it can clean up. */
    if (releaseSem (semid, WRITE_SEM) == -1)																
        errExit ("releaseSem");
	
    fprintf (stderr, "Received %d bytes from the writer in %d transfers.\n", nbytes, xfrs);
	
    exit (EXIT_SUCCESS);
}
