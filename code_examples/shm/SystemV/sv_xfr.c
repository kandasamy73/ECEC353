/* The progam demonstrates how to use UNIX V system calls to create a shared memory area and attach 
 * it to an address space. The client program can then access the shared object in 
 * the address space of this process. Access to the shared object is controlled using semaphores.
 *
 * Author: Naga Kandasamy
 * Date modified: July 17, 2018
 *
 * compile as follows: gcc -o sv_xfr sv_xfr.c sem_ops.c -std=c99 -Wall 
*/

#define _SVID_SOURCE

#include "shm_xfr.h"

extern void P (int);
extern void V (int);

/* Creates a shared memory area with the specified SHARED_MEMORY_KEY. */
SHM_STRUCT *
create_shm_area (key_t key, int *id)
{
    int shmid;
	SHM_STRUCT *ptr;

    	/* Create the shared memory segment of size SHARED_MEMORY_STRUCT. */
    	if ((shmid = shmget (key, sizeof(SHM_STRUCT), IPC_CREAT | 0666)) < 0) {
        	perror("shmget");
        	exit(EXIT_FAILURE);
    	}

    	/* Now attach the segment to our data space and return a pointer to the start of the shared area. */
    	if ((ptr = shmat (shmid, NULL, 0)) == NULL) {
        	perror("shmat");
        	exit(EXIT_FAILURE);
    	}
	
	*id = shmid;
	return ptr;
}

/* Create the semaphore that controls access to the shared memory area. Initialize to zero. */
int 
create_semaphore (int key)
{
	int id; 
	union semun {
    		int val;
    		struct semid_ds *buf;
    		ushort *array;
  	} argument;

  	argument.val = 0; 

  	/* Create the semaphore and assign it the value of SEMAPHORE_KEY. Give permissions to the 
	   everyone in the system to be able to access the semaphore. We typically can get an array 
	   of semaphores and the second argument specifies it. We are requesting a single value. */

  	if ((id = semget (key, 1, 0666 | IPC_CREAT)) < 0) {
		printf("semget");
        exit(EXIT_FAILURE);
  	}
  
  	/* Set the value of the zeroth element in the array of semaphores to 0. */
  	if (semctl (id, 0, SETVAL, argument) < 0){
		perror ("semctl");
		semctl (id, 0, IPC_RMID);
		return FALSE;
	}
  	else
		printf("Semaphore %d initialized. \n", key);

	return TRUE;
}

int 
main (int argc, char **argv)
{
	/* Create and initialize shared memory area. */
	int shm_id; /* ID of the shared memory */
	SHM_STRUCT *ptr = create_shm_area (SHM_KEY, &shm_id);

	/* Create and initialize the semaphore that protects shared memory. */
	int semaphore_id = create_semaphore (SEM_KEY); 

	while (TRUE) {
		P (SEM_KEY); /* Wait until a client process writes something to the buffer */
		if (!strcmp (ptr->buf, "quit")) 
            break; 
		else
			printf ("Client %d says %s. \n", ptr->client_id, ptr->buf);
	}

	/* Detach the shared memory from our address space. */
	shmdt ((void *)ptr);

	/* Detach the semaphore associated with the shared memory. */
	semctl (semaphore_id, 0, IPC_RMID);
    
    exit(EXIT_SUCCESS);
}


