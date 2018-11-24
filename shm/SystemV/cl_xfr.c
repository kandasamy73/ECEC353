/* The client program that communicates with the server using shared memory.
 *
 * Author: Naga Kandasamy
 * Date modified: July 17, 2018
 *
 * Compile as follows: gcc -o cl_xfr cl_xfr.c  sem_ops.c -std=c99 -Wall
*/

#define _SVID_SOURCE

#include "shm_xfr.h"

extern void P (int);
extern void V (int);

/* Locate the shared memory segment previously created by the server and attach the segment to our data space */
SHM_STRUCT 
*locate_shared_memory (key_t key)
{
	int shmid;
    SHM_STRUCT *ptr;

    /* Locate the segment previously created by the server. */
    if ((shmid = shmget (key, sizeof(SHM_STRUCT), 0666)) < 0) {
        perror ("shmget");
        exit (EXIT_FAILURE);
    }
    
    /* Attach the segment to our data space. */
    if ((ptr = shmat (shmid, NULL, 0)) == NULL) {
        perror ("shmat");
        exit (EXIT_FAILURE);
    }
	
    return ptr;
}

int 
main(int argc, char **argv)
{
	if (argc < 2) {
		printf ("Usage: ./cl_xfr client-id string \n");
		exit(0);
	}

	int client_id = atoi (argv[1]); // Obtain the client ID
	
	/* Locate shared memory */
	SHM_STRUCT *ptr = locate_shared_memory (SHM_KEY);

    /* Write to shared memory. */
    ptr->client_id = client_id;
	strcpy (ptr->buf, argv[2]);

	V (SEM_KEY); /* Signal the server */

    exit (EXIT_SUCCESS);
}

