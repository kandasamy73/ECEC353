/* The writer program that transfers user input from stdio to the shared memory area. Once 
 * the data has been transferred to shared memore, the reader process is signalled via a 
 * semaphore. The reader then obtains the data from the shared memory area and displays it 
 * on stdout.
 *
 * Author: Naga Kandasamy
 * Date: 7/9/2015
 *
 * Notes: Code taken from: Michael Kerrisk, The Linux Programming Interface, No Starch Press, 2010.
 *
 * Compile as follows: make clean && make
 *
 */
#define _SVID_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "semun.h"
#include "svshm_xfr.h"

void			/* Print out the error returned by the system and exit. */ 
errExit (char *message)
{
    perror (message);
    exit (EXIT_FAILURE);
}


int
main (int argc, char **argv)
{
    int semid, shmid, nbytes, xfrs;
    struct shmseg *shmp;
    union semun dummy;

    /* Create a set of two semaphores to be used by the reader and the writer. 
     * Sempahores are initialized so that the writer has first access to the 
     * shared memory area. Since the writer creates the shared memory segment, 
     * it must be started before the reader. 
    */
		  
    semid = semget (SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    if (semid == -1)
        errExit ("semget");
		
    if (initSemAvailable (semid, WRITE_SEM) == -1)
        errExit ("initSemAvailable");
		
    if (initSemInUse (semid, READ_SEM) == -1)
        errExit ("initSemInUse");

		  
    /* Create the shared memory segment and attach it to the writer's virtual address space
     * at an address chosen by the system. If all goes well, the pointer to that address 
     * is returned to us. */
		  
    shmid = shmget (SHM_KEY, sizeof (struct shmseg), IPC_CREAT | OBJ_PERMS);
    if (shmid == -1)
        errExit ("shmget");

    shmp = shmat (shmid, NULL, 0);
    if (shmp == (void *) -1)
        errExit ("shmat");
	  
    /* Transfer blocks of data from stdin to shared memory. */
    for (xfrs = 0, nbytes = 0; ; xfrs++, nbytes += shmp->count) {
        
        if (reserveSem (semid, WRITE_SEM) == -1)							/* Wait for our turn. */
            errExit ("reserveSem");
					 
        shmp->count = read (STDIN_FILENO, shmp->buf, BUF_SIZE);	/* Write data to shared memory. */
        if (shmp->count == -1)
            errExit ("read");

        if (releaseSem(semid, READ_SEM) == -1)								/* Give the reader a turn. */
            errExit("releaseSem");

        /* Check if an EOF has occurred (the user entered contrl+D). We test for this 
         * condition after giving the reader a turn so that it can also see the 0 
         * value in shmp->count. */
					
        if(shmp->count == 0)
            break;
   
    }

    /* Wait until the reader lets us have our turn. This way we know that the reader has 
     * finished and we can clean up the IPC objects that were created. */
	
    if (reserveSem(semid, WRITE_SEM) == -1)
        errExit ("reserveSem");
		
    if (semctl(semid, 0, IPC_RMID, dummy) == -1)
        errExit ("semctl");
		
    if (shmdt(shmp) == -1)
        errExit ("shmdt");
		
    if (shmctl (shmid, IPC_RMID, 0) == -1)
        errExit ("shmctl");

    fprintf (stderr, "Sent %d bytes to the reader using %d transfers.\n", nbytes, xfrs);
	
    exit (EXIT_SUCCESS);
}
