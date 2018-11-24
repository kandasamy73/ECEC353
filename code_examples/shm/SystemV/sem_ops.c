#define _SVID_SOURCE

#include "shm_xfr.h"

/* The probe or wait operation on the semaphore */
void 
P (int key)
{
	int id;
	struct sembuf operations[1]; /* An array of one operation to perform on the semaphore */

	/* Get the index of the semaphore using the provided key. */
  	if ((id = semget (key, 1, 0666)) < 0){
		perror ("semget");
		exit (EXIT_FAILURE);
  	}

    /* Perform the semaphore P operation. */
  	operations[0].sem_num = 0; /* We are operating on semaphore 0 in the array of semaphores */
  	operations[0].sem_op = -1; /* Subtract 1 to the semaphore value */
  	operations[0].sem_flg = 0; /* We will wait until the OS completes the requested semaphore operation */

  	/* Perform the P operation. */
  	if (semop (id, operations, 1) == 0)
		printf ("Server: P operation performed on semaphore %d. \n", key);
  	else 
		printf ("Server: P operation on semaphore %d failed. \n", key);

    return;
}

/* The signal operation on the semaphore. */
void 
V (int key)
{
	int id;
	struct sembuf operations[1]; /* An array of one operation to perform on the semaphore */

	/* Get the index of the semaphore using the provided key. */
  	if ((id = semget (key, 1, 0666)) < 0){
		perror("semget");
		exit (EXIT_FAILURE);
  	}

 	 /* Perform the semaphore V operation. */
  	operations[0].sem_num = 0; /* We are operating on semaphore 0 in the array of semaphores */
  	operations[0].sem_op = 1; /* Add 1 to the semaphore value */
  	operations[0].sem_flg = 0; /* We will wait until the OS completes the requested semaphore operation */

  	/* Perform the V operation. */
  	if (semop (id, operations, 1) == 0)
		printf ( "Server: V operation performed on semaphore %d. \n", key);
  	else 
		printf ("Server: V operation on semaphore %d failed. \n", key);

    return;
}

