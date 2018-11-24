/* The progam demonstrates how to use UNIX V system calls to create a shared memory area and attach it to an address space. 
 * A client program (see client.c) can then access the shared object in the address space of this process. Access to the shared object 
 * is controlled using semaphores.
 *
 * Author: Naga Kandasamy
 * Date modified: 01/14/2014
 *
 * compile as follows: gcc -o server server_code.c -std=c99 
*/

#define _SVID_SOURCE
#define _XOPEN_SOURCE

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include "shared_memory_struct.h"

#define FALSE 0
#define TRUE 1
#define SHARED_MEMORY_KEY 3228 // This is the well-known key for the shared memory area
#define SEMAPHORE_KEY 32281 // This semaphore synchronizes access to shared memory


/* The probe or wait operation on the semaphore */
void P(int key){
	int id;
	struct sembuf operations[1]; // An array of one operation to perform on the semaphore

	/* Get the index of the semaphore using the provided key. */
  	if ((id = semget(key, 1, 0666)) < 0){
		printf("Program cannot find the semaphore. Exiting. \n");
		exit(0);
  	}

 	 /* Perform the semaphore P operation. */
  	operations[0].sem_num = 0; // We are operating on semaphore 0 in the array of semaphores
  	operations[0].sem_op = -1; // Subtract 1 to the semaphore value 
  	operations[0].sem_flg = 0; // We will wait until the OS completes the requested semaphore operation 

  	/* Perform the P operation. */
  	if(semop(id, operations, 1) == 0)
		printf("Server: P operation performed on semaphore %d. \n", key);
  	else 
		printf("Server: P operation on semaphore %d failed. \n", key);

}

/* The signal operation on the semaphore. */
void V(int key){
	int id;
	struct sembuf operations[1]; // An array of one operation to perform on the semaphore

	/* Get the index of the semaphore using the provided key. */
  	if ((id = semget(key, 1, 0666)) < 0){
		printf("Program cannot find the semaphore. Exiting. \n");
		exit(0);
  	}

 	 /* Perform the semaphore V operation. */
  	operations[0].sem_num = 0; // We are operating on semaphore 0 in the array of semaphores
  	operations[0].sem_op = 1; // Add 1 to the semaphore value 
  	operations[0].sem_flg = 0; // We will wait until the OS completes the requested semaphore operation 

  	/* Perform the V operation. */
  	if(semop(id, operations, 1) == 0)
		printf("Server: V operation performed on semaphore %d. \n", key);
  	else 
		printf("Server: V operation on semaphore %d failed. \n", key);
}

/* Creates a shared memory area with the specified SHARED_MEMORY_KEY. */
SHARED_MEMORY_STRUCT *create_shared_memory_area(key_t key, int *id){
	int shmid;
	SHARED_MEMORY_STRUCT *ptr;

    	/* Create the shared memory segment of size SHARED_MEMORY_STRUCT. */
    	if ((shmid = shmget(key, sizeof(SHARED_MEMORY_STRUCT), IPC_CREAT | 0666)) < 0) {
        	perror("shmget");
        	exit(1);
    	}

    	/* Now attach the segment to our data space and return a pointer to the start of the shared area. */
    	if ((ptr = shmat(shmid, NULL, 0)) == NULL) {
        	perror("shmat");
        	exit(1);
    	}
	
	*id = shmid;
	return ptr;
}

/* Create the semaphore that controls access to the shared memory area. Initialize to zero. */
int create_semaphore(int key){
	int id; 

	union semun{
    		int val;
    		struct semid_ds *buf;
    		ushort *array;
  	} argument;

  	argument.val = 0; 

  	/* Create the semaphore and assign it the value of SEMAPHORE_KEY. Give permissions to the 
	   everyone in the system to be able to access the semaphore. We typically can get an array 
	   of semaphores and the second argument specifies it. We are requesting a single value. */

  	if ((id = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
		printf("Unable to obtain semaphore from the operating system. \n");
     		exit(0);
  	}
  
  	/* Set the value of the zeroth element in the array of semaphores to 0. */
  	if(semctl(id, 0, SETVAL, argument) < 0){
		printf("Cannot set semaphore value. Exiting. \n");
		semctl(id, 0, IPC_RMID);
		return FALSE;
	}
  	else
		printf("Semaphore %d initialized. \n", key);

	return TRUE;
}

int main(int argc, char **argv){
	/* Create and initialize shared memory area. */
	int shm_id; // ID of the shared memory
	SHARED_MEMORY_STRUCT *ptr = create_shared_memory_area(SHARED_MEMORY_KEY, &shm_id);

	/* Create and initialize the semaphore that protects shared memory. */
	int semaphore_id = create_semaphore(SEMAPHORE_KEY); 

	while(TRUE){
		P(SEMAPHORE_KEY); // Wait until a client process writes something to the buffer
		if(!strcmp(ptr->buffer, "quit")) break; 
		else
			printf("Client %d says %s. \n", ptr->client_id, ptr->buffer);
	}

	/* Detach the shared memory from our address space. */
	shmdt((void *)ptr);

	/* Detach the semaphore associated with the shared memory. */
	semctl(semaphore_id, 0, IPC_RMID);

    	exit(0);
}


